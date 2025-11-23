#include "ModePack.h"
#include "Scene.h"
#include "Spaceship.h"
#include "SkyBox.h"
#include "CamAnimController.h"
#include "CameraController.h"
#include "EditHelper.h"

namespace Level3ExitMode { std::deque<GameObject*> ControlObjectList; }

// level3은 마지막 레벨이므로 맵을 추가하지 않는다.
void Level3ExitMode::Start() {
	scene.SetupMode("level3_exit_mode", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "space";
	scene.AddObject(new SkyBox, "skybox", LAYER1);
	scene.AddObject(new Spaceship, "spaceship", LAYER1);
	auto cam = scene.AddObject(new CamAnimController(MODE_EXIT, 3), "cam_anim_cont", LAYER1);
	cam->SetCameraStartPosition({ 5.6, 3.3, -11.4 });
	cam->InputCenterPointPosition({ 0.f, 0.f, 0.f });
//	scene.AddObject(new CameraController, "cam_cont", LAYER1, true);
	//scene.AddObject(new EditHelper(false), "edit_helper", LAYER_UI1);
}

void Level3ExitMode::Destructor() {

}