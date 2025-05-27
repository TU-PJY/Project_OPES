#include "Scorpion.h"
#include "Scene.h"
#include "PickingUtil.h"

Scorpion::Scorpion(std::string mapName, XMFLOAT3& createPosition) {
	if (auto terrain = scene.Find(mapName); terrain)
		terrainUT = terrain->GetTerrain();
	SelectFBXAnimation(MESH.scorpion, keyframe);
	position = createPosition;
}

// ���Ͱ� �Ѿ˿� ������ �̺�Ʈ �߻�
// ���� ���� �� �ϳ��� �´´ٸ� �ǰ� �˻� ���� ����
// �÷��̾� ũ�ν� ������ ������ bool���� �޾� �ǰݵǾ��ٴ� �ǵ���� �����ش�.
// ��� ���ʹ� �� �Լ��� ������ �־�� �Ѵ�.
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
	// �ȱ� ����϶��� Inplace(��ġ����)�� ��Ų��.
	if (keyframe.compare("Walk") == 0)
		Transform::InPlace(TranslateMatrix, MESH.scorpion, false, false, true);
	Transform::Rotate(RotateMatrix, rotation);
	RenderFBX(MESH.scorpion, TEX.scorpion);
	UpdatePickMatrix();
}
