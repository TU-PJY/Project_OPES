#include "CenterBuilding.h"
#include "Scene.h"
#include "HP_Indicator.h"

CenterBuilding::CenterBuilding(std::string map_name, float height_offset) {
	if (auto terrain = scene.Find(map_name); terrain) {
		TerrainUtil terrainUtil;
		terrainUtil.InputPosition(position, height_offset);
		terrainUtil.ClampToTerrain(terrain->GetTerrain(), position, height_offset);

		aabb.Update(XMFLOAT3(position.x, position.y + height_offset, position.z), XMFLOAT3(8.0, 10.0, 9.0));
	}

	hpIndicator = scene.AddObject(new HP_Indicator(), "indicatorHP", LAYER1);
	if (hpIndicator) {
		hpIndicator->SetSize(3.0);
		hpIndicator->InputPosition(position, 10.0);
	}
}

CenterBuilding::~CenterBuilding() {
	if (hpIndicator)
		scene.DeleteObject(hpIndicator);
}

void CenterBuilding::Update(float Delta) {
	if(hpIndicator)
		hpIndicator->InputHP(totalHP, currentHP);
}

void CenterBuilding::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(ScaleMatrix, 1.5, 1.5, 1.5);
	Render3D(MESH.center_building, TEX.scifi);
	aabb.Render();
}

AABB CenterBuilding::GetAABB(){
	return aabb;
}

XMFLOAT3 CenterBuilding::GetPosition() {
	return XMFLOAT3(position.x, position.y + 5.0, position.z);
}

void CenterBuilding::GiveDamage(int Damage) {
	if (currentHP == 0)
		return;

	currentHP -= Damage;
	if (currentHP <= 0)
		currentHP = 0;
}
