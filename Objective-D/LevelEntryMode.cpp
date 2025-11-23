#include "ModePack.h"
#include "Scene.h"
#include "CamAnimController.h"
#include "SkyBox.h"
#include "Map1.h"
#include "CenterBuilding.h"
#include "FallingStone.h"

namespace Level1EntryMode { std::deque<GameObject*> ControlObjectList; }

void Level1EntryMode::Start() {
	scene.SetupMode("level1_entry_mode", Destructor, ControlObjectList);

	// 맵 추가
	GLOBAL.offsetFOV = 0.0;

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
	GLOBAL.mapName = "map1";
	scene.AddObject(new Map1, GLOBAL.mapName, LAYER1, true);
	auto center = scene.AddObject(new CenterBuilding(-2.0), "center_building", LAYER1);
	scene.AddObject(new FallingStone(xmfloat3(-114.3, 20.0, -40.9), 0), std::to_string(0), LAYER_STONE);
	scene.AddObject(new FallingStone(xmfloat3(-77.0, 10.0, 71.9), 1), std::to_string(1), LAYER_STONE);
	scene.AddObject(new FallingStone(xmfloat3(34.9, 10.0, -92.9), 2), std::to_string(2), LAYER_STONE);

	// 카메라 애니메이션 컨트롤러 추가
	auto camCont = scene.AddObject(new CamAnimController(MODE_ENTRY, 1), "cam_anim_cont", LAYER_UI1);
	camCont->SetCameraStartPosition({ 0.f, 30.f, 0.f });
	camCont->SetCameraFovRange(0.f, -45.f);
	auto centerPos = center->GetPosition();
	camCont->InputCenterPointPosition(centerPos);
}

void Level1EntryMode::Destructor() {

}