#include "ModePack.h"
#include "Scene.h"
#include "CamAnimController.h"
#include "SkyBox.h"
#include "Map1.h"
#include "OtherPlayer.h"
#include "CameraController.h"
#include "EditHelper.h"

namespace Level1ExitMode { std::deque<GameObject*> ControlObjectList; }

void Level1ExitMode::Start() {
	scene.SetupMode("level1_exit_mode", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map1";

	FOG_DATA FogData = {
		{0.63, 0.77, 0.98}, // Fog Color
		0.0,   //   padding1

		300.0, // Fog Start
		{0.0, 0.0, 0.0}, // padding2

		500.0, // FogEnd
		{0.0, 0.0, 0.0} // padding3
	};

	CBVUtil::Reset(GlobalSystem.CmdList, FogCBV);
	CBVUtil::Create(GlobalSystem.Device, &FogData, sizeof(FOG_DATA), FogCBV);

	scene.AddObject(new SkyBox, "skybox", LAYER1);
	auto map = scene.AddObject(new Map1, GLOBAL.mapName, LAYER1, false);
	GLOBAL.mapTerrain = map->GetTerrain();

	auto myself = scene.AddObject(new OtherPlayer(GLOBAL.myCharacter, 999999, "", true), "my_self", LAYER_PLAYER);
	myself->SetPosition(MAP1_DESTINATION);
	myself->SetRotation(-180.f);

	//scene.AddObject(new CameraController, "camcont", LAYER1, true);
	//scene.AddObject(new EditHelper, "editHelper", LAYER_UI2);

	auto camCont = scene.AddObject(new CamAnimController(MODE_EXIT, 1), "cam_anim_cont", LAYER_UI1);
	camCont->SetCameraStartPosition({ 115.8, 1.3, 91.5 });
	camCont->SetCameraFovRange(-30.f, 0.f);
	camCont->InputCenterPointPosition(MAP1_DESTINATION);
}

void Level1ExitMode::Destructor() {

}