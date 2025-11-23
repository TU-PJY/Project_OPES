#include "ModePack.h"
#include "Scene.h"
#include "Map3.h"
#include "SkyBox.h"
#include "CenterBuilding.h"
#include "CamAnimController.h"

namespace Level3EntryMode { std::deque<GameObject*> ControlObjectList; }

void Level3EntryMode::Start() {
	scene.SetupMode("level3_entry_mode", Destructor, ControlObjectList);

	// 맵 추가
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
	scene.AddObject(new Map3, "map3", LAYER1);
	auto center = scene.AddObject(new CenterBuilding(7.0), "center_building", LAYER1);

	auto camCont = scene.AddObject(new CamAnimController(MODE_ENTRY, 3), "cam_anim_cont", LAYER_UI1);
	camCont->SetCameraStartPosition({ 0.f, 30.f, 0.f });
	camCont->SetCameraFovRange(0.f, -45.f);
	auto centerPos = center->GetPosition();
	camCont->InputCenterPointPosition(centerPos);
}

void Level3EntryMode::Destructor() {

}