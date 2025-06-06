#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

#include "MuzzleFlash.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	FBX fbx{}, fbx2{};

	bool Init{};

	TestObject() {
	
	}

	void InputKey(KeyEvent& Event) {
		
	}

	void InputMouse(MouseEvent& Event) {

	}

	void Update(float Delta) {
		if (!Init) {
			fbx.SelectFBXMesh(MESH.scorpion);
			fbx.SelectAnimation("Walk");

			//fbx2.SelectFBXMesh(MESH.scorpion);
		//	fbx2.SelectAnimation("Death");
			Init = true;
		}

		fbx.UpdateAnimation(Delta, true);
		//std::cout << fbx.GetInplaceDelta() << std::endl;
	}

	void Render() {
		BeginRender();
		Transform::Move(TranslateMatrix, -fbx.GetInplaceDelta());
		RenderFBX(fbx, TEX.scorpion);
		//RenderFBX(MESH.scorpion, TEX.scorpion);

		//Transform::Move(TranslateMatrix, 4.0, 0.0, 0.0);
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