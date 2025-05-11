// cpp code
#include "ModePack.h"

#include "Map3.h"
#include "CameraController.h"

namespace Level3 { std::deque<GameObject*> ControlObjectList; }

void Level3::Start() {
	RegisterController();
	scene.RegisterModeName("Level3");

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new Map3, "map3", LAYER1, true);
}

void Level3::Destructor() {
}

void Level3::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };
	if (DEV_EXIT && Event.Type == WM_KEYDOWN && Event.Key == VK_ESCAPE)
		scene.Exit();
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputKey(Event);
}

void Level3::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputMouseMotion(Event);
}

void Level3::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputMouse(Event);
}

void Level3::RegisterController() {
	ControlObjectList.clear();
	scene.RegisterControlObjectList(ControlObjectList);
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}
