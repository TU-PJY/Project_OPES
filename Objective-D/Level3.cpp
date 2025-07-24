// cpp code
#include "ModePack.h"
#include "Map3.h"
#include "CameraController.h"
#include "EditHelper.h"
#include "Player1st.h"

namespace Level3 { std::deque<GameObject*> ControlObjectList; }

void Level3::Start() {
	bool editMode = false;

	scene.SetupMode("Level3", Destructor, ControlObjectList);

	GLOBAL.offsetFOV = 0.0;
	GLOBAL.mapName = "map3";
	GLOBAL.map3DefenseState = true;
	GLOBAL.map3DefenseEnemyRemained = 20;

	auto mapObject = scene.AddObject(new Map3, "map3", LAYER1, true);
	GLOBAL.mapTerrain = mapObject->GetTerrain();

	if (editMode) {
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
		scene.AddObject(new EditHelper, "editHelper", LAYERUI);
	}
	else
		scene.AddObject(new Player1st(CHARACTER_MG), "player", LAYER_PLAYER, true);

}

void Level3::Destructor() {
}