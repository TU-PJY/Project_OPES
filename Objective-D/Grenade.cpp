#include "Grenade.h"
#include "MathUtil.h"
#include "Explosion.h"

void SendPtoMDamagePacket(unsigned int playerID, unsigned int monsterID, int attackHp);
void SendGrenadePacket(float posX, float posY, float posZ, float rotX, float rotY, float rotZ);

Grenade::Grenade(const XMFLOAT3& createPosition, const XMFLOAT3& rotation, bool createFromServer) {
	position = createPosition;
	XMFLOAT3 direction = getDirectionFromRotation(rotation);
	velocity = XMFLOAT3(
		direction.x * 30.0,
		direction.y * 30.0,
		direction.z * 30.0
	);
	isStopped = false;

    this->createFromServer = createFromServer;

    if (!createFromServer)
        SendGrenadePacket(position.x, position.y, position.z, rotation.x, rotation.y, rotation.z);
}

void Grenade::updateBound() {
	grenadeBound.Update(position, 1.0);
    hitBound.Update(position, 30.0);
	terrainUtil.InputPosition(position, 0.49);
}

//— Closest point on an oriented box to a point —
XMVECTOR ClosestPointOnOBB(const BoundingOrientedBox& obb, FXMVECTOR point) {
    XMVECTOR center = XMLoadFloat3(&obb.Center);
    XMVECTOR delta = point - center;
    XMVECTOR orient = XMLoadFloat4(&obb.Orientation);

    XMVECTOR axisX = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), orient);
    XMVECTOR axisY = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), orient);
    XMVECTOR axisZ = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), orient);

    float dx = XMVectorGetX(XMVector3Dot(delta, axisX));
    float dy = XMVectorGetX(XMVector3Dot(delta, axisY));
    float dz = XMVectorGetX(XMVector3Dot(delta, axisZ));

    float cx = std::clamp(dx, -obb.Extents.x, obb.Extents.x);
    float cy = std::clamp(dy, -obb.Extents.y, obb.Extents.y);
    float cz = std::clamp(dz, -obb.Extents.z, obb.Extents.z);

    return center + axisX * cx + axisY * cy + axisZ * cz;
}

//— Swept‐sphere vs. OBB intersection (returns t in [0,1]) —
bool SweptSphereOBB(const BoundingOrientedBox& box, FXMVECTOR start, FXMVECTOR end, float radius, float& tOut) {
    // 1) Transform start/end into box’s local (AABB) space
    XMVECTOR center = XMLoadFloat3(&box.Center);
    XMVECTOR invOrient = XMQuaternionInverse(XMLoadFloat4(&box.Orientation));
    XMVECTOR sLocal = XMVector3Rotate(start - center, invOrient);
    XMVECTOR eLocal = XMVector3Rotate(end - center, invOrient);
    XMVECTOR dir = eLocal - sLocal;

    // 2) Expanded AABB bounds
    XMFLOAT3 e = box.Extents;
    float minX = -e.x - radius, maxX = e.x + radius;
    float minY = -e.y - radius, maxY = e.y + radius;
    float minZ = -e.z - radius, maxZ = e.z + radius;

    // 3) Slab‐based segment‐AABB test (param t in [0,1])
    float tMin = 0.0f, tMax = 1.0f;
    auto slab = [&](float s, float d, float mn, float mx) {
        if (fabsf(d) < 1e-6f) {
            return !(s < mn || s > mx);
        }
        else {
            float t1 = (mn - s) / d;
            float t2 = (mx - s) / d;
            float tn = std::min(t1, t2), tf = std::max(t1, t2);
            tMin = std::max(tMin, tn);
            tMax = std::min(tMax, tf);
            return tMin <= tMax;
        }
    };

    float sx = XMVectorGetX(sLocal), dx = XMVectorGetX(dir);
    if (!slab(sx, dx, minX, maxX)) return false;
    float sy = XMVectorGetY(sLocal), dy = XMVectorGetY(dir);
    if (!slab(sy, dy, minY, maxY)) return false;
    float sz = XMVectorGetZ(sLocal), dz = XMVectorGetZ(dir);
    if (!slab(sz, dz, minZ, maxZ)) return false;

    tOut = tMin;
    return true;
}

//— Grenade::updateMove with CCD, OBB + terrain collision —
void Grenade::updateMove(float Delta) {
    if (isStopped) return;

    // 1) Gravity
    velocity.y += -gravity * Delta * 4.0f;

    // 2) Compute prev & target positions
    XMVECTOR prevPos = XMLoadFloat3(&position);
    XMVECTOR velVec = XMLoadFloat3(&velocity);
    XMVECTOR moveVec = velVec * Delta;
    XMVECTOR targetPos = prevPos + moveVec;

    // 3) Sweep against each OBB
    for (auto& data : GLOBAL.mapOOBBdata) {
        const auto& box = data.oobb;
        float tHit;
        if (SweptSphereOBB(box, prevPos, targetPos, grenadeBound.sphere.Radius, tHit)
            && tHit <= 1.0f)
        {
            // — Move up to collision time —
            float hitTime = tHit * Delta;
            XMVECTOR hitPos = prevPos + velVec * hitTime;

            // — Compute contact normal —
            XMVECTOR closestPt = ClosestPointOnOBB(box, hitPos);
            XMVECTOR normal = XMVector3Normalize(hitPos - closestPt);

            // — Reflect velocity —
            float speed = XMVectorGetX(XMVector3Length(velVec));
            XMVECTOR reflDir = XMVector3Reflect(XMVector3Normalize(velVec), normal);
            XMVECTOR newVel = reflDir * speed;
            XMStoreFloat3(&velocity, newVel);

            // — Move remainder of frame —
            float remain = Delta - hitTime;
            XMVECTOR finalPos = hitPos + newVel * remain;
            XMStoreFloat3(&position, finalPos);
            XMStoreFloat3(&grenadeBound.sphere.Center, finalPos);

            goto DO_TERRAIN;
        }
    }

    // 4) No OBB hit → full move
    XMStoreFloat3(&position, targetPos);
    XMStoreFloat3(&grenadeBound.sphere.Center, targetPos);

DO_TERRAIN:
    // 5) Terrain collision (unchanged)
    if (terrainUtil.CheckCollision(GLOBAL.mapTerrain)) {
        XMFLOAT3 n3 = terrainUtil.GetNormalAtPoint(GLOBAL.mapTerrain);
        XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&n3));
        if (XMVectorGetY(normal) < 0) normal = -normal;

        XMVECTOR v = XMLoadFloat3(&velocity);
        XMVECTOR nComp = XMVector3Dot(v, normal) * normal;
        XMVECTOR tComp = v - nComp;

        XMVECTOR bounce = -nComp * restitution;
        XMVECTOR slide = tComp * friction;
        XMVECTOR result = slide + bounce;
        XMStoreFloat3(&velocity, result);

        float by = fabsf(XMVectorGetY(bounce));
        if (by < 2.0f) {
            isStopped = true;
            velocity = { 0,0,0 };
        }

        terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.5f);
    }
}

XMVECTOR Grenade::getNormalFromAngle(const XMFLOAT3& angleDeg) {
	// Pitch = X축 회전, Roll = Z축 회전 (Y축은 회전에 영향 없음)
	float pitchRad = XMConvertToRadians(angleDeg.x);
	float rollRad = XMConvertToRadians(angleDeg.z);

	// 기본 위쪽 벡터
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	// 회전 행렬 구성
	XMMATRIX rot = XMMatrixRotationRollPitchYaw(pitchRad, 0.0f, rollRad);
	return XMVector3TransformNormal(up, rot);  // 회전된 법선
}

XMFLOAT3 Grenade::getDirectionFromRotation(const XMFLOAT3& angleDeg) {
	float pitch = XMConvertToRadians(angleDeg.x); // up/down
	float yaw   = XMConvertToRadians(angleDeg.y); // left/right

	// DirectX 기준: Forward = -Z, Up = +Y
	XMVECTOR dir = XMVectorSet(
		cosf(pitch) * sinf(yaw),     // x
		sinf(pitch),                // y (중요: pitch 증가 시 아래로 향해야 하므로 부호 반전)
		cosf(pitch) * cosf(yaw),     // z
		0.0f
	);

	XMFLOAT3 result;
	XMStoreFloat3(&result, XMVector3Normalize(dir));
	return result;
}

XMFLOAT3 Grenade::getEulerFromVelocity(const XMFLOAT3& velocity) {
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&velocity));

	float yaw = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));       // Y축 회전
	float pitch = asinf(-XMVectorGetY(dir));                        // X축 회전 (y가 위쪽이므로 음수)
	float roll = 0.0f;                                              // 롤은 탄체에는 필요 없음

	XMFLOAT3 angleDeg;
	angleDeg.x = XMConvertToDegrees(pitch);  // Pitch
	angleDeg.y = XMConvertToDegrees(yaw);    // Yaw
	angleDeg.z = XMConvertToDegrees(roll);   // Roll
	return angleDeg;
}

void Grenade::updateCollision() {
	if (isStopped)
		return;
}

void Grenade::Update(float Delta) {
	updateBound();
	updateMove(Delta);
	updateCollision();

	// 3초가 지나면 폭발한다.
    // 가까울수록 강한 대미지를 가한다.
    // 타 클라이언트 유저가 던진 수류탄은 대미지를 주지 않는다.
	explodeTime += Delta;
	if (explodeTime >= 3.0) {
        if (!createFromServer) {
            size_t size = scene.LayerSize(LAYER_MONSTER);
            for (int i = 0; i < size; i++) {
                if (auto monster = scene.ReferLayer(LAYER_MONSTER, i); monster) {
                    if(monster->CheckHit(hitBound)) {
                        float distance = Math::CalcDistance3D(position, monster->GetPosition());
                        float t = (15.0 - distance) / 15.0;
                        t = std::clamp(t, 0.0f, 1.0f);
                        int damage = (int)(300.0 * t);
                        monster->GiveDamage(damage);
                        SendPtoMDamagePacket(GLOBAL.myID, monster->GetID(), damage);
                    }
                }
            }
        }

		scene.AddObject(new Explosion(position), "explosion", LAYER3);
		scene.DeleteObject(this);
	}
}

void Grenade::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, XMFLOAT3(3.0, 3.0, 3.0));
	Render3D(MESH.grenade, TEX.scifi);
}