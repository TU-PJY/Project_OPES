#include "CenterBuilding.h"
#include "Scene.h"

CenterBuilding::CenterBuilding(std::string map_name, float height_offset) {
	if (auto terrain = scene.Find(map_name); terrain) {
		terrainUT = terrain->GetTerrain();
		terrainUT.InputPosition(position, height_offset);
		terrainUT.SetHeightToTerrain(position);
	}
}

void CenterBuilding::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(ScaleMatrix, 0.01, 0.01, 0.01);
	Render3D(MESH.center_building, TEX.scifi);
}