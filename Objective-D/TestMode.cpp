#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	bool init{};

	TestObject() {
		
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN && Event.Key == 'P') {
			if (auto plantMonster = scene.Find("plantMonster"); plantMonster)
				plantMonster->GiveDamage(20);
		}
	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		if (!init) {
			scene.AddObject(new PlantMonster(XMFLOAT3(0.0, 0.0, 0.0), "", false), "plantMonster", LAYER2);
			init = true;
		}
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