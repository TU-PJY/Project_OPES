#include "ModePack.h"

#include "ClampUtil.h"
#include "CameraController.h"
#include "Map2.h"
#include "Player.h"
#include "CrossHair.h"


class OtherPlayer : public GameObject {
public:
	OtherPlayer() {
		SelectFBXAnimation(MESH.gazer, "Idle");
	}

	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	Vector vec{};

	void InputPosition(XMFLOAT3& value) override {
		position = value;
	}

	void InputRotation(XMFLOAT3& value) override {
		rotation = value;
	}

	void Update(float FrameTime) {
		UpdateFBXAnimation(MESH.gazer, FrameTime);
	}

	void Render() {
		BeginRender();
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(RotateMatrix, 0.0, 180.0 + rotation.y, 0.0);
		RenderFBX(MESH.gazer, TEX.gazer);
	}
};

class EnterEvent : public GameObject {
public:
	void Update(float FrameTime) {
		if (player_enter) {
			scene.AddObject(new OtherPlayer, std::to_string(enter_player_id), LAYER1);
			player_enter = false;
			std::cout << "PLAYER ENTERED +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";
		}
	}
};


namespace ServerTestMode { std::deque<GameObject*> ControlObjectList; }
void ServerTestMode::Start() {
	scene.SetupMode("ServerTestMode", Destructor, ControlObjectList);
	
	scene.AddObject(new EnterEvent, "enter_event", LAYER1);
	scene.AddObject(new Map2, "map2", LAYER1);
	scene.AddObject(new CrossHair, "crosshair", LAYER3);
	scene.AddObject(new Player("map2"), "player", LAYER1, true);
}

void ServerTestMode::Destructor() {

}