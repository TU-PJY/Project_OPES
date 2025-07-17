#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "PlantMonster.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	bool init{};
	FBX fbx{ MESH.heavy_idle };

	TestObject() {
		
	}

	void InputKey(KeyEvent& Event) {
	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		if (!init) {
			//fbx.SelectFBXMesh(MESH.heavy_idle);
			std::cout << fbx.GetCurrentAnimation() << std::endl;
			init = true;
		}
		fbx.UpdateAnimation(Delta);
	}

	void Render() {
		BeginRender();
		RenderFBX(fbx, TEX.scifi);
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