#include "ModePack.h"
#include "CameraController.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	TestObject() {
		camera.Move(0.0, 2.0, -6.0);
	}

	void InputKey(KeyEvent& Event) {

		if (Event.Type == WM_KEYDOWN) {

		}
	}
	void Update(float Delta) {
	}

	void Render() {
	}
};

namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	scene.SetupMode("TestMode", Destructor, ControlObjectList);

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "test_object", LAYER1, true);
}

void TestMode::Destructor() {
}