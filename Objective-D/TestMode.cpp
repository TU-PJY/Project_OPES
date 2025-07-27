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
			offset.forward = script.LoadDigitData(cat, "forward");
			offset.strafe = script.LoadDigitData(cat, "strafe");
			offset.height = script.LoadDigitData(cat, "height");
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
		rotation += Delta * 120.0;
	}

	void Render() {
		BeginRender();
		Transform::Rotate(RotateMatrix, 0.0, rotation, 0.0);
		RenderFBX(fbx, TEX.scifi);

		// heavy Idle flame
		BeginRender();
		xmfloat3 oft{};
		SetLightUse(DISABLE_LIGHT);
		oft = Math::CalcForwardOffset(oft, rotation, offset.forward, offset.height);
		oft = Math::CalcStrafeOffset(oft, rotation, offset.strafe, 0.0);
		Transform::Move(TranslateMatrix, oft);
		Transform::Rotate(RotateMatrix,0.0, rotation, 0.0);
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);

		// heavy move flame
		/*BeginRender();
		xmfloat3 offset{};
		SetLightUse(DISABLE_LIGHT);
		offset = Math::CalcForwardOffset(offset, 0.0, 0.45, 1.15);
		offset = Math::CalcStrafeOffset(offset, 0.0, 0.35, 0.0);
		Transform::Move(TranslateMatrix, offset);
		Render3D(MESH.gun_flame, TEX.gun_flame);
		Render3D(MESH.gun_flame_back, TEX.gun_flame_back);*/
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