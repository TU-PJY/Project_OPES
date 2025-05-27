#include "ModePack.h"
#include "CameraController.h"
#include "PickingUtil.h"

//테스트 작업을 위한 모드.

class TestObject : public GameObject {
public:
	OOBB frustum_oobb;
	bool picked{};

	TestObject() {
		SelectFBXAnimation(MESH.scorpion, "Walk");
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case '1':
				SelectFBXAnimation(MESH.scorpion, "Got Hit");
				break;
			case '2':
				SelectFBXAnimation(MESH.scorpion, "Walk");
				break;
			case '3':
				SelectFBXAnimation(MESH.scorpion, "Attack 1");
				break;
			case '4':
				SelectFBXAnimation(MESH.scorpion, "Death");
				break;
			}
		}
	}

	void InputMouse(MouseEvent& Event) {
		if (Event.Type == WM_LBUTTONDOWN) {
			bool pick{};
			std::cout << MESH.scorpion.MeshPart.size() << std::endl;
			for (auto const& M : MESH.scorpion.MeshPart) {
				if (PickingUtil::PickByViewport(mouse.x, mouse.y, this, M)) {
					pick = true;
					break;
				}
			}
			if(!pick)
				picked = false;
			else
				picked = true;
		}
	}

	void Update(float Delta) {
		UpdateFBXAnimation(MESH.scorpion, Delta * 2);
		frustum_oobb.Update(XMFLOAT3(0.0, 0.0, 0.0), XMFLOAT3(1.0, 1.0, 1.0), XMFLOAT3(0.0, 0.0, 0.0));
	}

	void Render() {
		BeginRender();
		if(picked)
			SetColor(0.0, 1.0, 0.0);
		XMFLOAT3 pos = fbxUtil.GetRootMoveDelta(MESH.scorpion, true);
		Transform::InPlace(TranslateMatrix, MESH.scorpion, false, false, true);
		
		RenderFBX(MESH.scorpion, TEX.scorpion);
		frustum_oobb.Render();
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