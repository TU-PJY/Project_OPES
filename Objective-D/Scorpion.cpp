#include "Scorpion.h"
#include "Scene.h"
#include "PickingUtil.h"
#include "HP_Indicator.h"
#include "ClampUtil.h"

Scorpion::Scorpion(std::string mapName, XMFLOAT3& createPosition, float Delay) {
	if (auto terrain = scene.Find(mapName); terrain)
		terrainUT = terrain->GetTerrain();
	//SelectFBXAnimation(MESH.scorpion, keyframe);
	position = createPosition;

	// ���� ��ü�� hp �ε������� �Ҵ�
	hp_ind = scene.AddObject(new HP_Indicator, "hp_ind", LAYER2);

	start_delay = Delay;
}

// ���Ͱ� �Ѿ˿� ������ �̺�Ʈ �߻�
// ���� ���� �� �ϳ��� �´´ٸ� �ǰ� �˻� ���� ����
// �÷��̾� ũ�ν� ������ ������ bool���� �޾� �ǰݵǾ��ٴ� �ǵ���� �����ش�.
// ��� ���ʹ� �� �Լ��� ������ �־�� �Ѵ�.
bool Scorpion::CheckHit(XMFLOAT2& checkPosition, int Damage) {
	for (auto const& M : MESH.scorpion.MeshPart) {
		if (PickingUtil::PickByViewport(checkPosition.x, checkPosition.y, this, M)) {
			hit_damage = Damage;
			current_hp -= hit_damage;
			Clamp::LimitValue(current_hp, 0.0, CLAMP_DIR_LESS);
			hit_state = true;

			return true;
		}
	}
	return false;
}

void Scorpion::SendPacket() {
	if (prev_hp != current_hp) {
		// send hp
		prev_hp = current_hp;
	}

	if (current_hp == 0) {
		// send death
	}
}

void Scorpion::GiveDamage(int damage) {
	current_hp -= damage;
	Clamp::LimitValue(current_hp, 0.0, CLAMP_DIR_LESS);
}

void Scorpion::Update(float Delta) {
	fbx.UpdateAnimation(Delta);

	terrainUT.InputPosition(position);
	terrainUT.ClampToTerrain(terrainUT, position, 0.0);

	// hp �ε������Ϳ� �ڽ��� ��ġ ����
	hp_ind->InputPosition(position, 2.0);

	// hp �ε������Ϳ� �ڽ��� hp����
	hp_ind->InputHP(full_hp, current_hp);

	// hp�� 0�� �� ��� �״� �ִϸ��̼��� ����Ѵ�.
	if (current_hp == 0 && !death_keyframe_selected) {
		//SelectFBXAnimation(MESH.scorpion, "Death");
		fbx.SelectAnimation("Death");
		death_keyframe_selected = true;
	}

	// �״� �ִϸ��̼��� ������ �����Ѵ�.
	if (death_keyframe_selected) {
		delete_delay += Delta;
		if (delete_delay >= 2.6) {
			scene.DeleteObject(hp_ind);
			scene.DeleteObject(this);
		}
	}
}


void Scorpion::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(RotateMatrix, rotation);
	RenderFBX(fbx, TEX.scorpion);
	UpdatePickMatrix();
}
