#include "ModePack.h"
#include "Scene.h"
#include "Map3.h"
#include "SkyBox.h"
#include "CamAnimController.h"

namespace Level3ExitMode { std::deque<GameObject*> ControlObjectList; }

void Level3ExitMode::Start() {
	scene.SetupMode("level3_exit_mode", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map3";

	FOG_DATA FogData = {
		{0.68, 0.28, 0.1}, // Fog Color
		0.0,   //   padding1

		500.0, // Fog Start
		{0.0, 0.0, 0.0}, // padding2

		900.0, // FogEnd
		{0.0, 0.0, 0.0} // padding3
	};

	CBVUtil::Reset(GlobalSystem.CmdList, FogCBV);
	CBVUtil::Create(GlobalSystem.Device, &FogData, sizeof(FOG_DATA), FogCBV);

	scene.AddObject(new SkyBox, "skybox", LAYER1);
	scene.AddObject(new Map3, "map", LAYER1, false);

	auto camCont = scene.AddObject(new CamAnimController(MODE_EXIT, 3), "cam_anim_cont", LAYER_UI1);
	camCont->SetCameraStartPosition({ MAP3_DESTINATION.x, MAP3_DESTINATION.y, MAP3_DESTINATION.z });
	camCont->SetCameraFovRange(-20.f, 0.f);
}

void Level3ExitMode::Destructor() {

}