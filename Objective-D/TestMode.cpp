#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "MuzzleFlash.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	void InputKey(KeyEvent& Event) {
		
	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		UpdateFBXAnimation(MESH.scorpion, Delta);
	}

	void Render() {
		BeginRender();
		RenderFBX(MESH.scorpion, TEX.scorpion);
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