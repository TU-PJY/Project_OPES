#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "Player.h"
#include "SampleMonster.h"
#include "CrossHair.h"

namespace Level2 {
	std::deque<GameObject*> ControlObjectList;
}

void Level2::Start() {
	RegisterController();
	scene.RegisterModeName("Level2");

	// ���� �ݵ�� Player���� ���� �߰��Ǿ�� �Ѵ�
	// �÷��̾� ��ü �����ڿ��� �� �����͸� �޾ƾ� �ϱ� ����
	scene.AddObject(new Map2, "map2", LAYER1, true);

	// �� ���� ��带 �����Ϸ��� dev_mode_enabled�� Ȱ��ȭ �Ѵ�.
	// ���� ��带 �����Ϸ��� dev_mode_enabled ��Ȱ��ȭ �Ѵ�.
	bool dev_mode_enabled = true;
	if (!dev_mode_enabled) {
		// ũ�ν����� �׻� ������ �ϹǷ� ���� ���̾ �߰�
		scene.AddObject(new CrossHair, "crosshair", LAYER3);
		scene.AddObject(new Player("map2"), "player", LAYER1, true);
	}
	else
		scene.AddObject(new CameraController, "camera_controller", LAYER1, true);

	scene.AddObject(new SampleMonster, "monster", LAYER1);

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
}

void Level2::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };
	if (DEV_EXIT && Event.Type == WM_KEYDOWN && Event.Key == VK_ESCAPE)
		scene.Exit();
	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputKey(Event);
}

void Level2::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);

	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputMouseMotion(Event);
}

void Level2::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object && !Object->DeleteCommand) Object->InputMouse(Event);
}

void Level2::RegisterController() {
	ControlObjectList.clear();
	scene.RegisterControlObjectList(ControlObjectList);
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}