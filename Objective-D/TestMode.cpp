#include "ModePack.h"
#include "CameraController.h"

//FBX �ִϸ��̼� ������ ���� �׽�Ʈ ���.

//Animation stack[0]: HoodDown
//Animation stack[1] : HoodUp
//Animation stack[2] : Armature | ArmatureAction
//Animation stack[3] : Armature | HoodDown
//Animation stack[4] : Armature | HoodUp
//Animation stack[5] : Armature | SpinAction

class TestObject : public GameObject {
public:
	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case '1':
				fbxUtil.SelectAnimation(MESH.Lain, "HoodDown");
				fbxUtil.ResetCurrentTime(MESH.Lain);
				break;

			case '2':
				fbxUtil.SelectAnimation(MESH.Lain, "HoodUp");
				fbxUtil.ResetCurrentTime(MESH.Lain);
				break;
			}
		}
	}

	void Update(float FT) override {
	    UpdateFBXAnimation(MESH.Lain, FT);
		UpdateFBXAnimation(MESH.steve, FT);
		UpdateFBXAnimation(MESH.man, FT);
	}

	void Render() override {
		BeginRender(RENDER_TYPE_3D);
		Transform::Move(TranslateMatrix, -5.0, 0.0, 5.0);
		Transform::Rotate(RotateMatrix, 0.0, 180.0, 0.0);
		RenderFBX(MESH.Lain, TEX.Lain);

		BeginRender(RENDER_TYPE_3D);
		Transform::Move(TranslateMatrix, 0.0, 2.5, 5.0);
		Transform::Rotate(RotateMatrix, 0.0, 180.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.35, 0.35, 0.35);
		RenderFBX(MESH.steve, TEX.steve);

		BeginRender(RENDER_TYPE_3D);
		Transform::Move(TranslateMatrix, 5.0, 0.0, 5.0);
		Transform::Scale(ScaleMatrix, 1.5, 1.5, 1.5);
		Transform::Rotate(RotateMatrix, 0.0, 180.0, 0.0);
		RenderFBX(MESH.man, TEX.man);
	}
};



void TestMode::Start() {
	std::vector<std::string> ControlObjectTag
	{
		"test_object",
		"camera_controller"
	};

	// �ʿ��� �۾� �߰�

	scene.AddObject(new CameraController, "camera_controller", LAYER1);
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
