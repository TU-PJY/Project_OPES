#include "ModePack.h"
#include "Title.h"

namespace TitleMode { std::deque<GameObject*> ControlObjectList; }

void TitleMode::Start() {
	scene.SetupMode("TitleMode", Destructor, ControlObjectList);
	scene.AddObject(new Title, "title", LAYER_UI2, true);
}

void TitleMode::Destructor() {
}
