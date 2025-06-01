#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	OOBB frustum_oobb;
	bool picked{};

	int state = 1;

	TestObject() {
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case '1':
				state = 1; break;
			case '2':
				state = 2; break;
			case '3':
				state = 3; break;
			case '4':
				state = 4; break;
			}
		}
	}

	void InputMouse(MouseEvent& Event) {
		
	}

	void Update(float Delta) {
		if(state == 1)
			UpdateFBXAnimation(MESH.heavy_idle, Delta);
		else if(state == 2)
			UpdateFBXAnimation(MESH.heavy_move, Delta);
		else if(state == 3)
			UpdateFBXAnimation(MESH.heavy_shoot, Delta * 10.0);
		else if(state == 4)
			UpdateFBXAnimation(MESH.heavy_death, Delta);
	}

	void Render() {
		BeginRender();
		if (state == 1)
			RenderFBX(MESH.heavy_idle, TEX.scifi);
		else if (state == 2)
			RenderFBX(MESH.heavy_move, TEX.scifi);
		else if (state == 3)
			RenderFBX(MESH.heavy_shoot, TEX.scifi);
		else if (state == 4)
			RenderFBX(MESH.heavy_death, TEX.scifi);
		UpdatePickMatrix();
	}
};

namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	scene.SetupMode("TestMode", Destructor, ControlObjectList);

	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "test_object", LAYER1, true);
}

void TestMode::Destructor() {
}