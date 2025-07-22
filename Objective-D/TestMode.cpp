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

	FBX fbx{ MESH.treant[2]};
	FBX fbx2{ MESH.troll };

	TestObject() {
		fbx2.SelectAnimation("Attack 3");
	}

	void InputKey(KeyEvent& Event) {

	}

	void InputMouse(MouseEvent& Event) {
	}

	void Update(float Delta) {
		fbx.UpdateAnimation(Delta);
		fbx2.UpdateAnimation(Delta);
	}

	void Render() {
		BeginRender();
		RenderFBX(fbx, TEX.treant);

		Transform::Move(TranslateMatrix, 5.0, 0.0, 0.0);
		RenderFBX(fbx2, TEX.troll);
	}
};



namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	//SetBackgroundColor(0.0, 0.0, 0.0);
	scene.SetupMode("TestMode", Destructor, ControlObjectList);
	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "testObject", LAYER1);
}

void TestMode::Destructor() {
}