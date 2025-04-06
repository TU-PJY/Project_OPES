#include "ModePack.h"

//FBX �ִϸ��̼� ������ ���� �׽�Ʈ ���.

class TestObject : public GameObject {
public:
	TestObject() {}

	void Update(float FT) override {

	}

	void Render() override {

	}
};

void TestMode::Start() {
	std::vector<std::string> ControlObjectTag
	{
		"test_object"
	};

	// �ʿ��� �۾� �߰�

	scene.AddObject(new TestObject, "test_object", LAYER1);

	AddControlObject(ControlObjectTag);
	RegisterController();
	scene.RegisterModeName("test_mode");
}

void TestMode::Destructor() {
	// ���⿡ ��� ���� �� �ʿ��� �۾� �߰� (���ҽ� �޸� ���� ��)
}

void TestMode::AddControlObject(std::vector<std::string> Vec) {
	ControlObjectList.clear();
	for (auto const& Object : Vec) {
		if (auto FindObject = scene.Find(Object); FindObject)
			ControlObjectList.emplace_back(FindObject);
	}
}

void TestMode::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputKey(Event);
}

void TestMode::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouseMotion(Event);
}

void TestMode::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouse(Event);
}

void TestMode::RegisterController() {
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}
