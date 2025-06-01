#include "CenterBuilding.h"
#include "Scene.h"

CenterBuilding::CenterBuilding(std::string map_name, float height_offset) {
	if (auto terrain = scene.Find(map_name); terrain) {
		terrainUT = terrain->GetTerrain();
		terrainUT.InputPosition(position, height_offset);
		terrainUT.SetHeightToTerrain(position);

		oobb.Update(XMFLOAT3(position.x, position.y + height_offset, position.z), XMFLOAT3(8.0, 10.0, 9.0), XMFLOAT3(0.0, 0.0, 0.0));
	}
}

void CenterBuilding::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Scale(ScaleMatrix, 1.5, 1.5, 1.5);
	Render3D(MESH.center_building, TEX.scifi);
	oobb.Render();
}

OOBB CenterBuilding::GetOOBB(){
	return oobb;
}
