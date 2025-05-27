#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "Player.h"
#include "CrossHair.h"

namespace Level2 { std::deque<GameObject*> ControlObjectList; }

void Level2::Start() {
	global_fov_offset = 0.0;
	scene.SetupMode("Level2", Destructor, ControlObjectList);

	// ���� �ݵ�� Player���� ���� �߰��Ǿ�� �Ѵ�
	// �÷��̾� ��ü �����ڿ��� �� �����͸� �޾ƾ� �ϱ� ����
	scene.AddObject(new Map2, "map2", LAYER1, true);

	// �� ���� ��带 �����Ϸ��� dev_mode_enabled�� Ȱ��ȭ �Ѵ�.
	// ���� ��带 �����Ϸ��� dev_mode_enabled ��Ȱ��ȭ �Ѵ�.
	bool dev_mode_enabled = true;
	if (!dev_mode_enabled) {
		// ũ�ν����� �׻� ������ �ϹǷ� ���� ���̾ �߰�
		scene.AddObject(new CrossHair, "crosshair", LAYER3);
		scene.AddObject(new Player("map2"), "player", LAYER1, true);
	}
	else
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
}