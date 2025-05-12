#include "ModePack.h"
#include "CameraController.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	TestObject() {
		camera.Move(0.0, 2.0, -6.0);
	}

	void InputKey(KeyEvent& Event) {
		// 시연용 임시 조작키
		if(Event.Type == WM_KEYDOWN && Event.Key == VK_LEFT)
			scene.SwitchMode(Level3::Start);

		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case 'I':
				SelectFBXAnimation(MESH.gazer, "Idle"); break;

			case 'T':
				SelectFBXAnimation(MESH.gazer, "Taunt"); break;

			case 'B':
				SelectFBXAnimation(MESH.gazer, "Bash01"); break;
			}
		}
	}
	void Update(float Delta) {
		UpdateFBXAnimation(MESH.gazer, Delta);
	}

	void Render() {
		BeginRender();
		Transform::Rotate(RotateMatrix, 0.0, 180.0, 0.0);
		RenderFBX(MESH.gazer, TEX.gazer);
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