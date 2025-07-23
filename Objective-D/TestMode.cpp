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

	float rotation{};

	TestObject() {
	}

	void InputKey(KeyEvent& Event) {

	}

	void InputMouse(MouseEvent& Event) {
	}

	void Update(float Delta) {
		rotation += Delta * 120.0;
	}

	void Render() {
		BeginRender();
		Transform::Scale(ScaleMatrix, 0.3, 0.3, 0.3);
		Render3D(MESH.turretTorso, TEX.turret);
		Render3D(MESH.turretBottom, TEX.turret);

		Transform::Rotate(TranslateMatrix, 0.0, 0.0, 0.0);
		Render3D(MESH.turretHead, TEX.turret);

		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		Transform::Rotate(TranslateMatrix, 0.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, 0.0, 0.23, -0.4);
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);

		//BeginRender();
		//RenderFBX(fbx, TEX.scifi);
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