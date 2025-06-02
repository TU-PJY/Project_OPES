#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "MuzzleFlash.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	GameObject* obj{};
	TestObject() {
		//obj = scene.AddObject(new MuzzleFlash(), "flash", LAYER3);
	}

	void InputKey(KeyEvent& Event) {
		
	}

	void InputMouse(MouseEvent& Event) {
		if (Event.Type == WM_LBUTTONDOWN)
			obj->EnableRender(0.1);
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