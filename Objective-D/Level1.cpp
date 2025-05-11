#include "ModePack.h"

#include "Map1.h"
#include "CameraController.h"


namespace Level1 {
	std::deque<GameObject*> ControlObjectList;
}

void Level1::Start() {
	RegisterController();
	scene.RegisterModeName("Level1");

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new Map1, "map1", LAYER1, true);
}

void Level1::Destructor() {
}

void Level1::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };
	if (DEV_EXIT && Event.Type == WM_KEYDOWN && Event.Key == VK_ESCAPE)
		scene.Exit();
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputKey(Event);
}

void Level1::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputMouseMotion(Event);
}

void Level1::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputMouse(Event);
}

void Level1::RegisterController() {
	ControlObjectList.clear();
	scene.RegisterControlObjectList(ControlObjectList);
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}


