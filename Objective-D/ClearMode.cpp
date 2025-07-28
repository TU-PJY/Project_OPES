#include "ModePack.h"
#include "ClearScreen.h"

namespace ClearMode { std::deque<GameObject*> ControlObjectList; }

void ClearMode::Start() {
	scene.SetupMode("ClearMode", Destructor, ControlObjectList);
	// add works here

	scene.AddObject(new ClearScreen, "cscreen", LAYERUI);
}

void ClearMode::Destructor() {
}
