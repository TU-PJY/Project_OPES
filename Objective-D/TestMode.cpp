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

#include "ScriptUtil.h"

GameObject* ind;

class TestObject : public GameObject {
public:
	FBX fbx{ MESH.engineer[1]};

	ScriptUtil script{};
	xmfloat3 position{ 10.0, 0.0, 5.0 };
	FlameOffset offset{};
		float rotation{};

	TestObject() {
		fbx.SetSpeed(1.0);

		xmfloat3 position{};
		
	}

	void Load() {
		script.Release();
		script.Load("Resources//Scripts//weapon//flamePosition.xml");

		auto load = [&](CategoryPtr cat) {
			offset.forward = script.LoadDigitData(cat, "z");
			offset.strafe = script.LoadDigitData(cat, "x");
			offset.height = script.LoadDigitData(cat, "y");
		};

		script.LoadAllData(load);
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {
			if (Event.Key == 'R')
				Load();
		}
	}

	void InputMouse(MouseEvent& Event) {
	}

	void Update(float Delta) {
		fbx.UpdateAnimation(Delta);
		//rotation += Delta * 120.0;
	}

	void Render() {
		BeginRender();
		Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(RotateMatrix, 0.0, rotation, 0.0);
		RenderFBX(fbx, TEX.scifi);

	
		//Transform::Rotate(TranslateMatrix , 0.0, rotation, 0.0);
	
		// heavy move flame
		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(TranslateMatrix, 0.0, rotation, 0.0);
		Transform::Move(TranslateMatrix, offset.strafe, offset.height, offset.forward);
		Transform::Scale(ScaleMatrix, 2.0, 2.0, 2.0);
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);
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