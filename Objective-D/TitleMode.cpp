#include "ModePack.h"
#include "Title.h"

namespace TitleMode { std::deque<GameObject*> ControlObjectList; }

void TitleMode::Start() {
	scene.SetupMode("TemplateMode", Destructor, ControlObjectList);
	scene.AddObject(new Title, "title", LAYERUI, true);
}

void TitleMode::Destructor() {
}
