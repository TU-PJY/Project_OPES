#include "ModePack.h"
#include "CameraController.h"

//FBX �ִϸ��̼� ������ ���� �׽�Ʈ ���.

class TestObject : public GameObject {
public:
	TestObject() {
		totaltime = GetMeshAnimationDuration();
		std::cout << totaltime << std::endl;
	}

	float time{};
	float totaltime{};

	double GetMeshAnimationDuration() {
		double maxTime = 0.0;
		for (const auto& channel : AnimationChannels) {
			if (!channel.keyframes.empty()) {
				double endTime = channel.keyframes.back().time;
				if (endTime > maxTime)
					maxTime = endTime;
			}
		}
		return maxTime;
	}

	void Update(float FT) override {
		time += FT;
		if (time >= totaltime)
			time = 0.0;
		for (auto& m : AnimatedMesh)
			m->UpdateSkinning(time);
	}

	void Render() override {
		BeginRender(RENDER_TYPE_3D);
		Transform::Move(TranslateMatrix, 0.0, 0.0, 5.0);
		Transform::Rotate(RotateMatrix, 0.0, 0.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.1, 0.1, 0.1);
		for (auto& m : AnimatedMesh)
			Render3D(m, TexRes.Man);
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
