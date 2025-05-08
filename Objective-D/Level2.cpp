// cpp code
#include "ModePack.h"

#include "Map2.h"
#include "CameraController.h"
#include "Player.h"
#include "SampleMonster.h"
#include "CrossHair.h"

void Level2::Start() {

	std::vector<std::string> ControlObjectTag
	{
		"camera_controller",
		"map2",
		"player"
	};

	// 맵이 반드시 Player보다 먼저 추가되어야 한다
	// 플레이어 객체 생성자에서 맵 데이터를 받아야 하기 때문
	scene.AddObject(new Map2, "map2", LAYER1);

	// 크로스헤어는 항상 보여야 하므로 상위 레이어에 추가
	scene.AddObject(new CrossHair, "crosshair", LAYER2);

	// 맵 편집 모드를 실행하려면 dev_mode_enabled를 활성화 한다.
	// 게임 모드를 실행하려면 dev_mode_enabled 비활성화 한다.
	bool dev_mode_enabled = false;
	if(!dev_mode_enabled)
		scene.AddObject(new Player("map2"), "player", LAYER2);
	else
		scene.AddObject(new CameraController, "camera_controller", LAYER1);

	scene.AddObject(new SampleMonster, "monster", LAYER1);

	AddControlObject(ControlObjectTag);
	RegisterController();
	scene.RegisterModeName("Level2");

	SetBackgroundColorRGB(135, 206, 235);
}

void Level2::Destructor() {
	// 여기에 모드 종료 시 필요한 작업 추가 (리소스 메모리 해제 등)
}

void Level2::AddControlObject(std::vector<std::string> Vec) {
	ControlObjectList.clear();
	for (auto const& Object : Vec) {
		if (auto FindObject = scene.Find(Object); FindObject)
			ControlObjectList.emplace_back(FindObject);
	}
}

void Level2::KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputKey(Event);
}

void Level2::MouseMotionController(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouseMotion(Event);
}

void Level2::MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };

	for (auto const& Object : ControlObjectList)
		if (Object) Object->InputMouse(Event);
}

void Level2::RegisterController() {
	scene.RegisterKeyController(KeyboardController);
	scene.RegisterMouseController(MouseController);
	scene.RegisterMouseMotionController(MouseMotionController);
	scene.RegisterDestructor(Destructor);
}


// header code
namespace Level2 {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}
