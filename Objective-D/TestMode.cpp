#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"
#include "TextUtil.h"
#include "Scorpion.h"
#include <string>

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:


	TestObject() {

	}

	void InputKey(KeyEvent& Event) {
	}

	void InputMouse(MouseEvent& Event) {

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
	scene.AddObject(new Scorpion(XMFLOAT3(0.0, 0.0, 0.0), "a"), "scorpion", LAYER1);
}

void TestMode::Destructor() {
}