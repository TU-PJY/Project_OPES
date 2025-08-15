#include "FallingStone.h"
#include "PickingUtil.h"

void SendPtoMDamagePacket(unsigned int monsterID, int attackHp);

FallingStone::FallingStone(const xmfloat3& createPosition, unsigned int createID) {
	position = createPosition;
	ID = createID;
	size = xmfloat3(0.5, 0.5, 0.5);

	Transform::Identity(TranslateMatrix);
	Transform::Identity(RotateMatrix);
	Transform::Identity(ScaleMatrix);

	Transform::Move(TranslateMatrix, position);
	Transform::Scale(TranslateMatrix, size);

	oobb.Update(MESH.LakeRockMesh[1], TranslateMatrix, RotateMatrix, ScaleMatrix, true);
	sphere.Update(position, 25.0);
}

void FallingStone::Update(float Delta) {
	if (hp > 0)
		return;

	// hp가 0이 되면 떨어지며, 땅에 닿으면 즉시 파괴되고, 주변에 있던 적들은 즉사한다.
	fallingAcc -= Delta * 0.5;
	position.y += fallingAcc;

	sphere.Update(position, 30.0);
	terrain.InputPosition(position , -3.0);

	if (terrain.CheckCollision(GLOBAL.mapTerrain)) {
		size_t size = scene.LayerSize(LAYER_MONSTER);
		for (int i = 0; i < size; i++) {
			if (auto monster = scene.ReferLayer(LAYER_MONSTER, i); monster) {
				if (monster->CheckHit(sphere)) {
					if (!GLOBAL.useServer)
						monster->GiveDamage(999);
					else {
						unsigned int ID = monster->GetID();
						SendPtoMDamagePacket(ID, 999);
					}
				}
			}
		}

		scene.DeleteObject(this);
	}
}

void FallingStone::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(TranslateMatrix, size);
	Render3D(MESH.LakeRockMesh[1], TEX.Palette2);

	sphere.Render();
	oobb.Render();
}

OOBB FallingStone::GetOOBB() {
	return oobb;
}

bool FallingStone::CheckHit(XMVECTOR& start, XMVECTOR& direction, float& distance) {
	if (hp == 0)
		return false;

	if (oobb.oobb.Intersects(start, direction, distance))
		return true;
	return false;
}

bool FallingStone::CheckHit(float& distance) {
	if (hp == 0)
		return false;

	if (PickingUtil::PickByViewportOOBB(xmfloat2(0.0, 0.0), distance, oobb))
		return true;
	return false;
}

unsigned int FallingStone::GetID() {
	return ID;
}

void FallingStone::GiveDamage(int damage) {
	hp -= damage;
	if (hp <= 0)
		hp = 0;
}
