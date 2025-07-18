#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"
#include "TextUtil.h"
#include <string>

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	Text text{ ALIGN_MIDDLE, HEIGHT_MIDDLE, XMFLOAT3(0.0, 1.0, 1.0) };

	TestObject() {
		text.EnableShadow();
		text.SetShadow(XMFLOAT2(0.01, -0.01), 0.5);
	}

	void InputKey(KeyEvent& Event) {

	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {

	}

	void Render() {
		text.Render(XMFLOAT2(0.0, 0.0), 0.3, "Hello World!");
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