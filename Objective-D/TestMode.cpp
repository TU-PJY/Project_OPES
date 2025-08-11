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
	FBX fbx{MESH.engineer[0]};

	Text playerName{ ALIGN_MIDDLE, HEIGHT_MIDDLE, {1.0f, 1.0f, 1.0f} };

	TestObject() {
		playerName.EnableStaticSize();
		
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
		Transform::Scale(ScaleMatrix, 3.0f, 3.0f, 3.0f);
		RenderFBX(fbx, TEX.scifi);

		BeginRender();
		Vector Vec{};
		float renderMultiply = Math::CalcDistance3D(XMFLOAT3(0.0, 6.0, 0.0), camera.GetPosition()) * 0.5;

		Transform::Move(TranslateMatrix, 0.0f, 6.0f, 0.0f);
		Math::BillboardLookAt(RotateMatrix, Vec, XMFLOAT3(0.0f, 6.0f , 0.0f) , camera.GetPosition());
		Transform::Scale(ScaleMatrix, 0.03 * 8.0 * renderMultiply, 0.05 * renderMultiply, 1.0);
		Render3D(SYSRES.BillboardMesh, TEX.ColorTex, 0.6, DEPTH_TEST_NONE);

		playerName.Render3D(XMFLOAT3(0.0f, 6.0f, 0.0f), 0.03, "Player1");
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