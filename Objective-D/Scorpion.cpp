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

	Math::InitVector(vec);

	fbx.SelectAnimation("Walk");

}

// ���Ͱ� �Ѿ˿� ������ �̺�Ʈ �߻�
// ���� ���� �� �ϳ��� �´´ٸ� �ǰ� �˻� ���� ����
// �÷��̾� ũ�ν� ������ ������ bool���� �޾� �ǰݵǾ��ٴ� �ǵ���� �����ش�.
// ��� ���ʹ� �� �Լ��� ������ �־�� �Ѵ�.
bool Scorpion::CheckHit(XMFLOAT2& checkPosition, int Damage) {
	if (PickingUtil::PickByViewportOOBB(checkPosition.x, checkPosition.y, oobb)) {
		hit_damage = Damage;
		current_hp -= hit_damage;
		Clamp::LimitValue(current_hp, 0.0, CLAMP_DIR_LESS);
		hit_state = true;
		render_particle = true;
		particle_alpha = 1.0;

		return true;
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

bool Scorpion::ChangeHP(int HP) {
	if (current_hp > HP) {
		current_hp = HP;
		render_particle = true;
		particle_alpha = 1.0;
		return true;
	}


	return false;
}

int Scorpion::GetID() {
	return ID;
}

OOBB Scorpion::GetOOBB() {
	return oobb;
}

XMFLOAT3 Scorpion::GetPosition() {
	return position;
}

bool Scorpion::GetDeathState() {
	return death_keyframe_selected;
}

void Scorpion::Update(float Delta) {
	if (render_particle) {
		particle_alpha -= Delta * 10.0;
		if (particle_alpha <= 0.0) {
			particle_alpha = 0.0;
			render_particle = false;
		}
	}

	current_delay += Delta;
	if (current_delay >= start_delay)
		activate_state = true;

	if (activate_state) {
		if (avoid_state) {
			avoid_time += Delta;
			if (avoid_time >= 1.0) {
				avoid_state = false;
				avoid_time = 0.0;
			}

			if(avoid_dir == 1)
				rotation = std::lerp(rotation, dest_rotation + 95, 15.0 * Delta);
			else
				rotation = std::lerp(rotation, dest_rotation - 95, 15.0 * Delta);
		}

		else {
			dest_rotation = Math::CalcDegree2D(position.z, position.x, -120.0, -120.0);
			Math::Normalize2DAngleTo360(dest_rotation);
			rotation = std::lerp(rotation, dest_rotation, 5.0 * Delta);
		}
		
		Math::UpdateVector(vec, XMFLOAT3(0.0, rotation, 0.0));

		if (move_state && !attack_state) {
			Math::MoveForward(position, rotation, 6.0 * Delta);
			oobb.Update(position, XMFLOAT3(0.5, 0.8, 0.8), XMFLOAT3(0.0, rotation, 0.0));

			// ������ �浹�ϸ� ����
			if (auto building = scene.Find("center_building"); building) {
				if (oobb.CheckCollision(building->GetOOBB())) {
					Math::MoveForward(position, rotation, -8.0 * Delta);
					attack_state = true;
				}
			}

			// �ٸ� ���Ϳ� �浹�ϸ� �Ͻ� ����
			if (!avoid_state) {
				size_t Size = scene.LayerSize(LAYER1);
				for (int i = 0; i < Size; i++) {
					if (auto object = scene.FindMulti("scorpion", LAYER1, i)) {
						if (object != this) {
							if (!object->GetDeathState() && oobb.CheckCollision(object->GetOOBB())) {
								move_state = false;
								break;
							}
							else
								move_state = true;
						}
					}
				}
			}
		}

		if (attack_state && !attack_selected && !death_keyframe_selected) {
			fbx.SelectAnimation("Attack 1");
			attack_selected = true;
		}

		if (!move_state && !idle_selected && !attack_selected && !death_keyframe_selected) {
			fbx.SelectAnimation("Idle");
			idle_selected = true;
			move_selected = false;
		}

		if (move_state && !move_selected && !attack_selected && !death_keyframe_selected) {
			fbx.SelectAnimation("Walk");
			idle_selected = false;
			move_selected = true;
		}

		if (fbx.GetCurrentAnimation() == "Walk") {
			fbx.SetSpeed(5.0);
			fbx.UpdateAnimation(Delta, true);
		}
		else {
			fbx.SetSpeed(1.0);
			fbx.UpdateAnimation(Delta);
		}


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
			move_state = false;
		}

		// �״� �ִϸ��̼��� ������ �����Ѵ�.
		if (death_keyframe_selected) {
			delete_delay += Delta;
			if (delete_delay >= 2.6) {
				scene.DeleteObject(hp_ind);
				scene.DeleteObject(this);
			}
		}

		SendPacket(Delta);
	}
}


void Scorpion::Render() {
	if (!activate_state)
		return;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Transform::Rotate(TranslateMatrix, 0.0, rotation, 0.0);
	Transform::Move(TranslateMatrix, -fbx.GetInplaceDelta());
	
	RenderFBX(fbx, TEX.scorpion);
	UpdatePickMatrix();

	if (render_particle) {
		std::vector<XMFLOAT3> pos = blood.GetPositions();
		for (auto& P : pos) {
			BeginRender();
			Transform::Move(TranslateMatrix, position.x, position.y + 1.0, position.z - 1.0);
			Transform::Move(TranslateMatrix, P);
			Math::BillboardLookAt(RotateMatrix, blood.GetVector(), position, camera.GetPosition());
			SetColor(1.0, 0.0, 0.0);
			SetLightUse(DISABLE_LIGHT);
			Render3D(SysRes.BillboardMesh, TEX.muzzle_particle, particle_alpha);
		}
	}

	oobb.Render();
}
