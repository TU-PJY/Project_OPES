#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"
#include "TextUtil.h"
#include "Scorpion.h"
#include <string>

#include "PlayerIndicator.h"
#include "RandomUpgrade.h"
#include "BuffDebuffIndicator.h"
#include "PlayerTag.h"

#include "Imp.h"
#include "Gazer.h"

#include "Lobby.h"
//테스트 작업을 위한 모드.

#include "ScriptUtil.h"

GameObject* ind;

class TestObject : public GameObject {
public:
	GameObject* tag{};
	TestObject() {
		tag = scene.AddObject(new PlayerTag("Player1"), "tag", LAYER_UI1);
	}

	void InputKey(KeyEvent& Event) {

	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		tag->InputPosition(XMFLOAT3( 0.0, 0.0, 0.0 ));
	}

	void Render() {
	}
};

namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {

	for (int i = 0; i < 2; i++) {
		GLOBAL.buff[i] = true;
		GLOBAL.deBuff[i] = true;
	}

	//SetBackgroundColor(0.0, 0.0, 0.0);
	scene.SetupMode("TestMode", Destructor, ControlObjectList);
	scene.AddObject(new CameraController(true), "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "testObject", LAYER1, true);
	scene.AddObject(new BuffDebuffIndicator, "ran", LAYER_UI1);
	
//	scene.AddObject(new Lobby, "lobby", LAYERUI, true);
	//ind = scene.AddObject(new PlayerIndicator(CHARACTER_MG), "ind", LAYER1);
	//scene.AddObject(new EngineerIndicator, "ind", LAYER1);
}

void TestMode::Destructor() {
}