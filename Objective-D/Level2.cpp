// cpp code
#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "Player.h"
#include "SampleMonster.h"
#include "CrossHair.h"

void Level2::Start() {

	std::vector<std::string> ControlObjectTag
	{
		"camera_controller",
		"map2",
		"player"
	};

	// ���� �ݵ�� Player���� ���� �߰��Ǿ�� �Ѵ�
	// �÷��̾� ��ü �����ڿ��� �� �����͸� �޾ƾ� �ϱ� ����
	scene.AddObject(new Map2, "map2", LAYER1);

	// ũ�ν����� �׻� ������ �ϹǷ� ���� ���̾ �߰�
	scene.AddObject(new CrossHair, "crosshair", LAYER2);

	// �� ���� ��带 �����Ϸ��� dev_mode_enabled�� Ȱ��ȭ �Ѵ�.
	// ���� ��带 �����Ϸ��� dev_mode_enabled ��Ȱ��ȭ �Ѵ�.
	bool dev_mode_enabled = false;
	if(!dev_mode_enabled)
		scene.AddObject(new Player("map2"), "player", LAYER2);
	else
		scene.AddObject(new CameraController, "camera_controller", LAYER1);

	scene.AddObject(new SampleMonster, "monster", LAYER1);

	AddControlObject(ControlObjectTag);
	RegisterController();
	scene.RegisterModeName("Level2");

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
	// ���⿡ ��� ���� �� �ʿ��� �۾� �߰� (���ҽ� �޸� ���� ��)
}

void Level2::AddControlObject(std::vector<std::string> Vec) {
	ControlObjectList.clear();
	for (auto const& Object : Vec) {
		if (auto FindObject = scene.Find(Object); FindObject)
			ControlObjectList.emplace_back(FindObject);
	}
}

void Level2::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputKey(Event);
}

void Level2::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouseMotion(Event);
}

void Level2::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouse(Event);
}

void Level2::RegisterController() {
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}


// header code
namespace Level2 {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}
