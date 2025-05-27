#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "Player.h"
#include "CrossHair.h"

namespace Level2 { std::deque<GameObject*> ControlObjectList; }

void Level2::Start() {
	global_fov_offset = 0.0;
	scene.SetupMode("Level2", Destructor, ControlObjectList);

	// 맵이 반드시 Player보다 먼저 추가되어야 한다
	// 플레이어 객체 생성자에서 맵 데이터를 받아야 하기 때문
	scene.AddObject(new Map2, "map2", LAYER1, true);

	// 맵 편집 모드를 실행하려면 dev_mode_enabled를 활성화 한다.
	// 게임 모드를 실행하려면 dev_mode_enabled 비활성화 한다.
	bool dev_mode_enabled = true;
	if (!dev_mode_enabled) {
		// 크로스헤어는 항상 보여야 하므로 상위 레이어에 추가
		scene.AddObject(new CrossHair, "crosshair", LAYER3);
		scene.AddObject(new Player("map2"), "player", LAYER1, true);
	}
	else
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
}