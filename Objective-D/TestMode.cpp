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
	XMFLOAT3 size{ 3.0, 3.0, 3.0 };
	XMFLOAT3 position{};
	BoundSphere bs{};

	TestObject() {

	}

	void InputKey(KeyEvent& Event) {
	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		fbx.UpdateAnimation(Delta);
		XMFLOAT3 boundPosition = Math::CalcForwardOffset(position, 45.0, 4.0, size.y * 0.5);
	//	bs.Update(boundPosition, 3.0);
	}

	void Render() {
		BeginRender();
		//Transform::Scale(ScaleMatrix, size);
		////Transform::Rotate(RotateMatrix, 0.0, 45.0, 0.0);
		RenderFBX(fbx, TEX.scifi);
		//bs.Render();
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