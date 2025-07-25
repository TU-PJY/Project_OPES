#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"
#include "TextUtil.h"
#include "Scorpion.h"
#include <string>

#include "Imp.h"
#include "Gazer.h"
//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	FBX fbx{ MESH.gazer };
	FBX fbx2{ MESH.imp };

	OOBB oobb{};
	OOBB oobb2{};

	Text text{};

	int index{}, index2{};

	// gazer: Idle, Bash01, MoveForward, 3
	// imp: Idle, Attack02, Walk ,4

	TestObject() {
		fbx.SelectAnimation("Death");
		fbx2.SelectAnimation("Death");
		oobb.SetUpdateFrequency(24);
		oobb2.SetUpdateFrequency(24);
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case VK_UP:
				index++;
				if (index > fbx.GetMeshCount() - 1)
					index = 0;
				break;
			case VK_RIGHT:
				index2++;
				if (index2 > fbx2.GetMeshCount() - 1)
					index2 = 0;
				break;
			}
		}
	}

	void InputMouse(MouseEvent& Event) {
	}

	void Update(float Delta) {
		fbx.UpdateAnimation(Delta);
		fbx2.UpdateAnimation(Delta);
		oobb.UpdateDelta(Delta);
		oobb2.UpdateDelta(Delta);
	}

	void Render() {
		BeginRender();
		Transform::Move(TranslateMatrix, -2.0, 0.0, 0.0);
		//Transform::Move(TranslateMatrix, -fbx.GetInplaceDelta());
		RenderFBX(fbx, TEX.gazer);
		oobb.UpdateAnimated(fbx, TranslateMatrix, RotateMatrix, ScaleMatrix, index);
		oobb.Render();

		BeginRender();
		Transform::Move(TranslateMatrix, 2.0, 0.0, 0.0);
		//Transform::Move(TranslateMatrix, -fbx2.GetInplaceDelta());
		RenderFBX(fbx2, TEX.imp);
		oobb2.UpdateAnimated(fbx2, TranslateMatrix, RotateMatrix, ScaleMatrix, index2);
		oobb2.Render();

		std::string renderStr = "Gazer: " + std::to_string(index) + " " + "Imp: " + std::to_string(index2);
		text.Render(xmfloat2(0.0, 0.0), 0.3, renderStr);
	}
};



namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	//SetBackgroundColor(0.0, 0.0, 0.0);
	scene.SetupMode("TestMode", Destructor, ControlObjectList);
	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	//scene.AddObject(new TestObject, "testObject", LAYER1, true);

	scene.AddObject(new Gazer(xmfloat3(-2.0, 0.0, 0.0), 0), "gazer", LAYER1);
	scene.AddObject(new Imp(xmfloat3(2.0, 0.0, 0.0), 0), "imp", LAYER1);
}

void TestMode::Destructor() {
}