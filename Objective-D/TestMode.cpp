#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"
#include "TextUtil.h"
#include "Treant.h"
#include <string>

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:


	TestObject() {

	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {
			GameObject* treant{};

			if (treant = scene.Find("treant"); treant) {
				switch (Event.Key) {
				case 'Z':
					treant->SetState(0);
					break;
				case 'X':
					treant->SetState(1);
					break;
				case 'C':
					treant->SetState(2);
					break;
				case 'V':
					treant->SetState(3);
					break;
				}
			}
		}
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
	scene.AddObject(new Treant(XMFLOAT3(0.0, 0.0, 0.0), "map1"), "treant", LAYER1);
}

void TestMode::Destructor() {
}