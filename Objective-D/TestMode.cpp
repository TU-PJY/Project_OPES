#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"
#include "TextUtil.h"
#include "Scorpion.h"
#include <string>

#include "PlayerIndicator.h"

#include "Imp.h"
#include "Gazer.h"

#include "Lobby.h"
//테스트 작업을 위한 모드.

GameObject* ind;

class TestObject : public GameObject {
public:
	FBX fbx{ MESH.troll};
	BoundSphere sphere{};

	TestObject() {
		fbx.SelectAnimation("Attack 3");
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {

		}
	}

	void InputMouse(MouseEvent& Event) {
	}

	void Update(float Delta) {
		fbx.UpdateAnimation(Delta);
		sphere.Update(xmfloat3(0.0, 2.0 * 0.5, 3.0), 4.0);
	}

	void Render() {
		BeginRender();
		Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
		RenderFBX(fbx, TEX.troll);

		sphere.Render();
	}
};

namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	//SetBackgroundColor(0.0, 0.0, 0.0);
	scene.SetupMode("TestMode", Destructor, ControlObjectList);
	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "testObject", LAYER1, true);
//	scene.AddObject(new Lobby, "lobby", LAYERUI, true);
	//ind = scene.AddObject(new PlayerIndicator(CHARACTER_MG), "ind", LAYER1);
	//scene.AddObject(new EngineerIndicator, "ind", LAYER1);
}

void TestMode::Destructor() {
}