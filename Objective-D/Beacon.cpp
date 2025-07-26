#include "Beacon.h"
#include "CameraUtil.h"
#include "TerrainUtil.h"
#include "HP_Indicator.h"

void SendEngineerInstallPacket(int type, unsigned int ID, float rotY, float posX, float posY, float posZ);


Beacon::Beacon(const xmfloat3& createPosition, float rotation, bool createFromServer) {
	position = createPosition;
	this->rotation.y = rotation;
	TerrainUtil terrainUtil{};
	terrainUtil.InputPosition(position);
	terrainUtil.ClampToTerrain(GLOBAL.mapTerrain, position, 0.0);
	frustumBound.Update(position, xmfloat3(1.0, 1.5, 1.0), this->rotation);

	hpInd = scene.AddObject(new HP_Indicator, "hpInd", LAYER3);
	if (hpInd) {
		hpInd->InputPosition(position, 2.0);
		hpInd->SetIndColor(xmfloat3(0.0, 1.0, 0.0));
	}

	if(!createFromServer)
		SendEngineerInstallPacket(BEACON_ID, 0, rotation, position.x, position.y, position.z);
}

Beacon::~Beacon() {
	if (hpInd)
		scene.DeleteObject(hpInd);
}

void Beacon::Update(float Delta) {
	inFrustum = camera.CheckFrustum(frustumBound);

	currentHP -= Delta;
	if (currentHP <= 0.0) {
		scene.DeleteObject(this);
		if (hpInd) {
			scene.DeleteObject(hpInd);
			hpInd = nullptr;
		}
		return;
	}

	if (hpInd)
		hpInd->InputHP((int)BEACON_DURABILITY, (int)currentHP);

	healZone.Update(position, 10.0);
	
	// 0.5초 간격으로 플레이어들을 5씩 회복시킨다.
	currentDelay -= Delta;
	if (currentDelay <= 0.0) {
		size_t size = scene.LayerSize(LAYER_PLAYER);
		for (int i = 0; i < size; i++) {
			if (auto player = scene.ReferLayer(LAYER_PLAYER, i); player) {
				if(healZone.CheckCollision(player->GetOOBB()))
					player->GiveHeal(5);
			}
		}

		currentDelay += BEACON_HEAL_DELAY;
	}
}

void Beacon::Render() {
	if (!inFrustum)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
	Render3D(MESH.beacon, TEX.scifi);

	frustumBound.Render();
	healZone.Render();
}