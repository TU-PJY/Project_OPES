#include "ModePack.h"
#include "Scene.h"
#include "CamAnimController.h"
#include "SkyBox.h"
#include "Map2.h"
#include "CenterBuilding.h"

namespace Level2EntryMode { std::deque<GameObject*> ControlObjectList; }

void Level2EntryMode::Start() {
	scene.SetupMode("level2_entry_mode", Destructor, ControlObjectList);

	// 맵 추가
	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map2";

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
	scene.AddObject(new Map2, "map", LAYER1, GLOBAL.editMode);
	auto center = scene.AddObject(new CenterBuilding(-2.0), "center_building", LAYER1);

	auto camCont = scene.AddObject(new CamAnimController(MODE_ENTRY, 2), "cam_anim_cont", LAYER_UI1);
	camCont->SetCameraStartPosition({ 0.f, 30.f, 0.f });
	camCont->SetCameraFovRange(0.f, -45.f);
	auto centerPos = center->GetPosition();
	camCont->InputCenterPointPosition(centerPos);
}

void Level2EntryMode::Destructor() {

}