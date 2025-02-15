#include "ModePack.h"

#include "Map1.h"
#include "CameraController.h"

// �ش� cpp���ϰ� h������ Ư���� ��带 �����ϰ�, �ش� ��忡 �����ϴ� ��ü���� ��Ʈ���ϱ� ���� �����̴�.
// �ݵ�� cpp, h���Ϸ� �и��Ǿ��־�� �ϸ�, �� ��忡 ���� ���ӽ����̽��� ���еǾ���Ѵ�.
// ���Ӹ��::Start() ������ ��� �Լ� ���ӽ����̽��� ��� �̸��� �ۼ��ϴ� ���� �����Ѵ�
// �Ʒ��� ���� ������ ��� ���� �Լ� �� ��Ʈ�ѷ� �Լ��� �ۼ����ֱ⸸ �ϸ� �����ӿ�ũ�� �˾Ƽ� ���ش�.
// ���α׷� ���� �� �Ʒ��� �Լ����� [ DirectX 3D ] -> [ Main ] -> Scene.cpp�� �ִ� Init() �Լ����� ����ȴ�.
// �Ǵ� ��� ���� �� ����ȴ�.
// ���� ������ Scene.cpp���� �����ϵ��� �Ѵ�.

// ������ ��� �ڵ� �ۼ��� ���� [ Template ] ���Ϳ� ���ø��� ����� �ξ����� ������ �� �Լ� �̸��� ���ӽ����̽� �̸��� �ٲٸ� �ȴ�.

void Level1::Start() {
	std::vector<std::string> ControlObjectTag 
	{
		"camera_controller",
		"map1"
	};

	// �ʿ��� ��ü �߰�
	scene.AddObject(new CameraController, "camera_controller", LAYER1);
	scene.AddObject(new Map1, "map1", LAYER1);
	
	// ��Ʈ�ѷ��� ������ ������Ʈ ������ ����
	AddControlObject(ControlObjectTag);

	// scene�� ��Ʈ�ѷ� �� ��� �Ҹ��� ���
	RegisterController();

	// ��� �̸��� Scene�� ���
	scene.RegisterModeName("Level1");
}

void Level1::Destructor() {
	// ���⿡ ��� ���� �� �ʿ��� �۾� �߰� (���ҽ� �޸� ���� ��)
}

void Level1::AddControlObject(std::vector<std::string> Vec) {
	ControlObjectList.clear();
	for (auto const& Object : Vec) {
		if (auto FindObject = scene.Find(Object); FindObject)
			ControlObjectList.emplace_back(FindObject);
	}
}

void Level1::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };

	// esc ���� �� ���α׷� ����
	if (Event.Type == WM_KEYDOWN && Event.Key == VK_ESCAPE)
		// ���α׷��� �����ϴ� Scene ��� �Լ�
		scene.Exit();

	// ��ü�� Ű���� �Է�
	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputKey(Event);
}

//  ���콺 ����� ������ ��ü �����ͷ� �����Ѵ�
void Level1::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };

	// ���콺 ��ǥ�� ����Ʈ ��ǥ�� ��ȯ�Ѵ�.
	mouse.UpdateMousePosition(hWnd);

	// ��ü�� ���콺 ��� �Է�
	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouseMotion(Event);
}

// ���콺 ��ư Ŭ�� �̺�Ʈ�� ������ ��ü �����ͷ� �����Ѵ�
void Level1::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };

	// ��ü�� ���콺 �Է�
	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouse(Event);
}

// scene�� ��Ʈ�ѷ� �� ��� �Ҹ��� ���
void Level1::RegisterController() {
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}