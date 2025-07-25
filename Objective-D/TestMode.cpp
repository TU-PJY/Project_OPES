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
//테스트 작업을 위한 모드.

GameObject* ind;

class TestObject : public GameObject {
public:
	TestObject() {
	}

	void InputKey(KeyEvent& Event) {
		if (Event.Type == WM_KEYDOWN) {

		}
	}

	void InputMouse(MouseEvent& Event) {
		if (Event.Type == WM_MOUSEWHEEL) {
			int delta = GET_WHEEL_DELTA_WPARAM(Event.wParam); // 휠 스크롤량 (+120, -120 등)

			if (delta > 0) {
				ind->ScrollRight();
			}
			else {
				ind->ScrollLeft();
			}

		}
	}

	void Update(float Delta) {

	}

	void Render() {
		BeginRender(RENDER_TYPE_2D);
		Transform::Scale2D(ScaleMatrix, 4.0, 4.0);
		Render2D(TEX.scope);

		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, 1.0 * ASPECT - 0.1, 0.0);
		Transform::Scale2D(ScaleMatrix, 1.0, 4.0);
		Render2D(TEX.ColorTex);

		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.1, 0.0);
		Transform::Scale2D(ScaleMatrix, 1.0, 4.0);
		Render2D(TEX.ColorTex);
	}
};

namespace TestMode { std::deque<GameObject*> ControlObjectList; }

void TestMode::Start() {
	//SetBackgroundColor(0.0, 0.0, 0.0);
	scene.SetupMode("TestMode", Destructor, ControlObjectList);
	scene.AddObject(new CameraController, "camera_controller", LAYER1, true);
	scene.AddObject(new TestObject, "testObject", LAYER1, true);
	//ind = scene.AddObject(new PlayerIndicator(CHARACTER_MG), "ind", LAYER1);
	//scene.AddObject(new EngineerIndicator, "ind", LAYER1);
}

void TestMode::Destructor() {
}