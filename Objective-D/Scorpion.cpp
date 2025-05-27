#include "Scorpion.h"
#include "Scene.h"
#include "PickingUtil.h"

Scorpion::Scorpion(std::string mapName, XMFLOAT3& createPosition) {
	if (auto terrain = scene.Find(mapName); terrain)
		terrainUT = terrain->GetTerrain();
	SelectFBXAnimation(MESH.scorpion, keyframe);
	position = createPosition;
}

// 몬스터가 총알에 맞으면 이벤트 발생
// 여러 부위 중 하나라도 맞는다면 피격 검사 조기 종료
// 플레이어 크로스 헤어에서는 리턴한 bool값을 받아 피격되었다는 피드백을 보여준다.
// 모든 몬스터는 이 함수를 가지고 있어야 한다.
bool Scorpion::CheckHit(XMFLOAT2& checkPosition) {
	for (auto const& M : MESH.scorpion.MeshPart) {
		if (PickingUtil::PickByViewport(checkPosition.x, checkPosition.y, this, M)) {
			hitState = true;
			std::cout << "scorpion hit\n";
			return true;
		}
	}
	return false;
}

void Scorpion::Update(float Delta) {
	UpdateFBXAnimation(MESH.scorpion, Delta);
	terrainUT.InputPosition(position);
	terrainUT.ClampToTerrain(terrainUT, position, 0.0);
}


void Scorpion::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	// 걷기 모션일때는 Inplace(위치고정)를 시킨다.
	if (keyframe.compare("Walk") == 0)
		Transform::InPlace(TranslateMatrix, MESH.scorpion, false, false, true);
	Transform::Rotate(RotateMatrix, rotation);
	RenderFBX(MESH.scorpion, TEX.scorpion);
	UpdatePickMatrix();
}
