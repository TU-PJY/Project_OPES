#include "Turret.h"
#include "CameraUtil.h"
#include "ClampUtil.h"
#include "Bullet.h"
#include "HP_Indicator.h"

void SendEngineerInstallPacket(int type, unsigned int ID, float rotY, float posX, float posY, float posZ);
void SendPtoMDamagePacket(unsigned int playerID, unsigned int monsterID, int attackHp);

Turret::Turret(const xmfloat3& createPosition, float createRotation, bool createFromServer) {
	position = createPosition;
	rotation.y = createRotation;
	createdByServer = createFromServer;
	hpInd = scene.AddObject(new HP_Indicator, "indicator", LAYER3);

	if (!createFromServer)
		SendEngineerInstallPacket(CONSTRUCT_TURRET, 0, rotation.y, position.x, position.y, position.z);
}

Turret::~Turret() {
	if (hpInd)
		scene.DeleteObject(hpInd);
}

void Turret::updateBound() {
	xmfloat3 turretPosition = xmfloat3(position.x, position.y + 0.5, position.z);
	frustumBound.Update(turretPosition, xmfloat3(3.0, 3.0, 3.0));
	inFrustum = camera.CheckFrustum(frustumBound);

	lookRange.Update(position, 30.0);
	hitBox.Update(turretPosition, xmfloat3(3.0, 3.0, 3.0), xmfloat3(0.0, rotation.y, 0.0));
}

void Turret::Update(float Delta) {
	static int count;

	currentHP -= Delta;

	if (hpInd) {
		hpInd->InputPosition(position, 2.0);
		hpInd->InputHP(20, (int)currentHP);
	}

	if (currentHP <= 0.0) {
		scene.DeleteObject(hpInd);
		hpInd = nullptr;
		scene.DeleteObject(this);
		return;
	}

	updateBound();

	size_t size = scene.LayerSize(LAYER_MONSTER);

	if (!targeted) {
		bool isLook = true;
		for (int i = 0; i < size; i++) {
			if (auto monster = scene.ReferLayer(LAYER_MONSTER, i); monster) {
				if (!monster->GetDeathState() && monster->CheckHit(lookRange) && monster->GetBehaviorState()) {
					xmfloat3 lookPosition = monster->GetPosition();
					Ray newRay = Math::CalcRayVector(position, lookPosition);

					for (auto& O : GLOBAL.mapOOBBdata) {
						if (Math::CheckRayCollision(newRay, O)) {
							isLook = false;
							break;
						}
					}

					if (isLook) {
						currentTargetID = monster->GetID();
						std::cout << currentTargetID << std::endl;
						target = monster;
						targeted = true;
					}
				}
			}
		}
	}

	currentShootDelay -= Delta;
	Clamp::LimitValue(currentShootDelay, 0.0, CLAMP_DIR_LESS);

	flameRenderTime -= Delta;
	Clamp::LimitValue(flameRenderTime, 0.0, CLAMP_DIR_LESS);

	if (targeted) {
		if (target) {
			xmfloat3 lookPosition = target->GetPosition();
			Ray newRay = Math::CalcRayVector(position, lookPosition);

			headRotationDest = Math::CalcDegree3D(position, lookPosition);
			headRotationDest.y -= rotation.y;
			headRotationDest.x *= -1;
			Math::Normalize2DAngleTo360(headRotationDest.x);
			Math::Normalize2DAngleTo360(headRotationDest.y);

			for (auto& O : GLOBAL.mapOOBBdata) {
				if (Math::CheckRayCollision(newRay, O)) {
					targeted = false;
					break;
				}
			}

			if (target->GetDeathState())
				targeted = false;

			if (currentShootDelay <= 0.0) {
				if (!createdByServer) {
					target->GiveDamage(10);
					SendPtoMDamagePacket(0, currentTargetID, 10);
				}
				currentShootDelay += 0.2;
				flameRenderTime += 0.05;
			}
		}
		else
			targeted = false;
	}

	else {
		headRotationDest.y += 90.0 * Delta;
		headRotationDest.x = 0.0;
	}
	
	headRotation.x = Math::LerpDegrees(headRotation.x, headRotationDest.x, 15.0 * Delta);
	headRotation.y = Math::LerpDegrees(headRotation.y, headRotationDest.y, 15.0 * Delta);
}

void Turret::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, rotation);
	Render3D(MESH.turretTorso, TEX.turret);
	Render3D(MESH.turretBottom, TEX.turret);

	Transform::Rotate(TranslateMatrix, headRotation);
	Render3D(MESH.turretHead, TEX.turret);

	if (flameRenderTime > 0.0) {
		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(TranslateMatrix, rotation);
		Transform::Rotate(TranslateMatrix, headRotation);
		Transform::Scale(TranslateMatrix, 4.0, 4.0, 4.0);
		Transform::Move(TranslateMatrix, 0.0, 0.18, -0.4);
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
	}
}
