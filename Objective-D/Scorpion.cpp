#include "Scorpion.h"
#include "Scene.h"
#include "PickingUtil.h"
#include "HP_Indicator.h"
#include "ClampUtil.h"
#include <string>

void SendPlayer2MonsterPacket(unsigned int monsterID, unsigned int hp);

Scorpion::Scorpion(std::string mapName, XMFLOAT3& createPosition, float Delay, int ID) {
	if (auto terrain = scene.Find(mapName); terrain)
		terrainUT = terrain->GetTerrain();
	//SelectFBXAnimation(MESH.scorpion, keyframe);
	position = createPosition;

	// ���� ��ü�� hp �ε������� �Ҵ�
	hp_ind = scene.AddObject(new HP_Indicator, "hp_ind", LAYER2);

	start_delay = Delay;

	this->ID = ID;
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

void Scorpion::SendPacket(float Delta) {
	send_delay += Delta;

	if (current_hp < full_hp) {
		if (send_delay >= 0.025) {
			SendPlayer2MonsterPacket(ID, current_hp);
			float over_time = send_delay - 0.025;
			send_delay = over_time;
		}
	}
}

void Scorpion::ChangeHP(int HP) {
	current_hp = HP;
}

int Scorpion::GetID() {
	return ID;
}

void Scorpion::Update(float Delta) {
	SendPacket(Delta);

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
