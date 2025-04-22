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
	//void InputKey(KeyEvent& Event) {
	//	if (Event.Type == WM_KEYDOWN) {
	//		switch (Event.Key) {
	//		case '1':
	//			fbxUtil.SelectAnimation(MeshRes.Lain, "HoodDown");
	//			fbxUtil.ResetCurrentTime(MeshRes.Lain);
	//			break;

	//		case '2':
	//			fbxUtil.SelectAnimation(MeshRes.Lain, "HoodUp");
	//			fbxUtil.ResetCurrentTime(MeshRes.Lain);
	//			break;
	//		}
	//	}
	//}

	void Update(float FT) override {
	    UpdateFBXAnimation(MeshRes.steve, FT);
	}

	void Render() override {
		BeginRender(RENDER_TYPE_3D);
		Transform::Move(TranslateMatrix, 0.0, 0.0, 10.0);
	//	Transform::Rotate(RotateMatrix, -90.0, 180.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.01, 0.01, 0.01);
		RenderFBX(MeshRes.steve, TexRes.steve);
	}
};



void TestMode::Start() {
	std::vector<std::string> ControlObjectTag
	{
		"test_object",
		"camera_controller"
	};

	// 필요한 작업 추가

	scene.AddObject(new CameraController, "camera_controller", LAYER1);
	scene.AddObject(new TestObject, "test_object", LAYER1);

	AddControlObject(ControlObjectTag);
	RegisterController();
	scene.RegisterModeName("test_mode");
}

void TestMode::Destructor() {
	// 여기에 모드 종료 시 필요한 작업 추가 (리소스 메모리 해제 등)
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
