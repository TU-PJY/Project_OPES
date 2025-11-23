#include "ModePack.h"
#include "Scene.h"

namespace Level1EntryMode { std::deque<GameObject*> ControlObjectList; }

void Level1EntryMode::Start() {
	scene.SetupMode("level1_entry_mode", Destructor, ControlObjectList);
	// add works here
}

void Level1EntryMode::Destructor() {

}