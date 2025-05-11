#include "ModePack.h"
#include "CameraController.h"

//FBX 애니메이션 구현을 위한 테스트 모드.

//Animation stack[0]: HoodDown
//Animation stack[1] : HoodUp
//Animation stack[2] : Armature | ArmatureAction
//Animation stack[3] : Armature | HoodDown
//Animation stack[4] : Armature | HoodUp
//Animation stack[5] : Armature | SpinAction



class TestObject : public GameObject {
public:
	TestObject() {
		camera.Move(0.0, 2.0, -6.0);
	}

	void InputKey(KeyEvent& Event) {
		// 시연용 임시 조작키
		if(Event.Type == WM_KEYDOWN && Event.Key == VK_LEFT)
			scene.SwitchMode(Level3::Start);
	}
	void Update(float Delta) {
		UpdateFBXAnimation(MESH.gazer, Delta);
	}

	void Render() {
		BeginRender();
		Transform::Rotate(RotateMatrix, 0.0, 180.0, 0.0);
		RenderFBX(MESH.gazer, TEX.gazer);
	}
};

namespace TestMode {
	std::deque<GameObject*> ControlObjectList;
}

void TestMode::Start() {
	RegisterController();
	scene.RegisterModeName("test_mode");

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "test_object", LAYER1, true);
}

void TestMode::Destructor() {
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
	ControlObjectList.clear();
	scene.RegisterControlObjectList(ControlObjectList);
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}
