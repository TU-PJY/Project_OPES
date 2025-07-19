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
	FBX fbx{ MESH.heavyIdle };
	OOBB oobb{};
	BoundSphere bs{};

	TestObject() {
		oobb.SetUpdateFrequency(24);
	}

	void InputKey(KeyEvent& Event) {
	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		oobb.UpdateDelta(Delta);
		bs.Update(XMFLOAT3(0.0, 4.5, 0.0), 1.0);
		fbx.UpdateAnimation(Delta);
	}

	void Render() {
		BeginRender();
		Transform::Scale(ScaleMatrix, XMFLOAT3(3.0, 3.0, 3.0));
		RenderFBX(fbx, TEX.scifi);
		oobb.UpdateAnimated(fbx, TranslateMatrix, RotateMatrix, ScaleMatrix, 0);
		oobb.Render();
	//	bs.Render();
	}
};

namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	scene.SetupMode("TestMode", Destructor, ControlObjectList);
	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "test_object", LAYER1, true);
	//scene.AddObject(new Scorpion(XMFLOAT3(0.0, 0.0, 0.0), "a"), "scorpion", LAYER1);
}

void TestMode::Destructor() {
}