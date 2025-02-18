// cpp code
#include "ModePack.h"

#include "Map3.h"
#include "CameraController.h"

void Level3::Start() {
	std::vector<std::string> ControlObjectTag
	{
		"map3",
		"camera_controller"
	};

	scene.AddObject(new CameraController, "camera_controller", LAYER1);
	scene.AddObject(new Map3, "map3", LAYER1);

	// 필요한 작업 추가

	AddControlObject(ControlObjectTag);
	RegisterController();
	scene.RegisterModeName("Level3");
}

void Level3::Destructor() {
	// 여기에 모드 종료 시 필요한 작업 추가 (리소스 메모리 해제 등)
}

void Level3::AddControlObject(std::vector<std::string> Vec) {
	ControlObjectList.clear();
	for (auto const& Object : Vec) {
		if (auto FindObject = scene.Find(Object); FindObject)
			ControlObjectList.emplace_back(FindObject);
	}
}

void Level3::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputKey(Event);
}

void Level3::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouseMotion(Event);
}

void Level3::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouse(Event);
}

void Level3::RegisterController() {
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}


// header code
namespace Level3 {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}
