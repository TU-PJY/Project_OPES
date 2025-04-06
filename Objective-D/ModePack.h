#pragma once
#include "Scene.h"
#include "MouseUtil.h"

extern std::vector<GameObject*> ControlObjectList;

// ���⿡ ��� ���ӽ����̽� �Լ����� ��Ƽ� �����Ѵ�. -> ��忡 ���Ǵ� ���� ������ ���̱� ����
namespace Level1 {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}

namespace Level2 {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}

namespace Level3 {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}


// FBX �ִϸ��̼� ������ ���� �׽�Ʈ ���. ���� ���� �� ��Ȱ��ȭ�Ѵ�.
namespace TestMode {
	void Start();
	void Destructor();
	void AddControlObject(std::vector<std::string> Vec);
	void KeyboardController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void MouseMotionController(HWND hWnd);
	void MouseController(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void RegisterController();
}

