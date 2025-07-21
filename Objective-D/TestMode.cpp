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

	FBX fbx{ MESH.explosion };

	TestObject() {

	}

	void InputKey(KeyEvent& Event) {

	}

	void InputMouse(MouseEvent& Event) {
	}

	void Update(float Delta) {
		fbx.UpdateAnimation(Delta);
	}

	void Render() {
		BeginRender();
		SetColor(1.0, 0.8, 0.0);
		RenderFBX(fbx, TEX.ColorTex);

		SetColor(1.0, 0.0, 0.0);
		Transform::Rotate(RotateMatrix, 45.0, 180.0, 45.0);
		RenderFBX(fbx, TEX.ColorTex);

		SetColor(1.0, 0.5, 0.0);
		Transform::Rotate(RotateMatrix, 180.0, 0.0, 45.0);
		RenderFBX(fbx, TEX.ColorTex);

		SetColor(1.0, 1.0, 0.0);
		Transform::Rotate(RotateMatrix, 180.0, 180.0, 180.0);
		RenderFBX(fbx, TEX.ColorTex);
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