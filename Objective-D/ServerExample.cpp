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

	void Update(float FrameTime) {
		UpdateFBXAnimation(MESH.gazer, FrameTime);
	}

	void Render() {
		BeginRender();
		Transform::Rotate(RotateMatrix, 0.0, 180.0, 0.0);
		RenderFBX(MESH.gazer, TEX.gazer);
	}
};

class EnterEvent : public GameObject {
public:
	void Update(float FrameTime) {
		if (player_enter) {
			scene.AddObject(new OtherPlayer, "other", LAYER1);
			player_enter = false;
		}
	}
};


namespace ServerTestMode { std::deque<GameObject*> ControlObjectList; }
void ServerTestMode::Start() {
	scene.SetupMode("ServerTestMode", Destructor, ControlObjectList);
	
	scene.AddObject(new Map2, "map2", LAYER1);
	scene.AddObject(new CrossHair, "crosshair", LAYER3);
	scene.AddObject(new Player("map2"), "player", LAYER1, true);
}

void ServerTestMode::Destructor() {

}