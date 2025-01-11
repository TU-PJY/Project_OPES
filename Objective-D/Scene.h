#pragma once
#include "Config.h"
#include "GameObject.h"
#include "ShaderUtil.h"
#include <deque>
#include <array>
#include <map>
#include <vector>

typedef void(*Function)(void);
using ObjectRange = std::multimap<std::string, GameObject*>::iterator;
constexpr int Layers = static_cast<int>(EOL);

class Scene {
private:
	std::array<std::deque<GameObject*>, Layers> ObjectList{};
	std::multimap<std::string, GameObject*> ObjectIndex{};
	std::array<std::vector<int>, Layers> DeleteLocation{};
	int CurrentReferPosition{};
	int SceneCommandCount{};
	bool CommandExist{};

	std::string RunningMode{};
	void (*MouseControllerPtr)(HWND, UINT, WPARAM, LPARAM) {};
	void (*MouseMotionControllerPtr)(HWND) {};
	void (*KeyboardControllerPtr)(HWND, UINT, WPARAM, LPARAM) {};

	Function DestructorBuffer{};
	
public:
	std::string GetMode();
	void RegisterDestructor(Function Destructor);
	void ReleaseDestructor();
	void Init(Function ModeFunction);
	void SwitchMode(Function ModeFunction);
	void RegisterModeName(std::string ModeName);
	void RegisterKeyController(void(*FunctionPtr)(HWND, UINT, WPARAM, LPARAM));
	void RegisterMouseController(void(*FunctionPtr)(HWND, UINT, WPARAM, LPARAM));
	void RegisterMouseMotionController(void(*FunctionPtr)(HWND));

	void InputKeyMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void InputMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void InputMouseMotionMessage(HWND hWnd);
	void InputMouse(std::string ObjectTag, MouseEvent& Event);
	void InputKey(std::string ObjectTag, KeyEvent& Event);
	void InputMouseMotion(std::string ObjectTag, MotionEvent& Event);
	void Routine(float FT, ID3D12GraphicsCommandList* CmdList);
	void AddObject(GameObject*&& Object, std::string Tag, int InputLayer);
	void DeleteObject(GameObject* Object);
	GameObject* Find(std::string Tag);
	std::pair<ObjectRange, ObjectRange> EqualRange(std::string Tag);
	void CompleteCommand();
	ID3D12RootSignature* CreateObjectRootSignature(ID3D12Device* Device);
	ID3D12RootSignature* GetGraphicsRootSignature();
	void PrepareRender(ID3D12GraphicsCommandList* CmdList);
	void ReleaseObjects();
	void Exit();

private:
	void AddLocation(int Layer, int Position);
	void ProcessObjectCommand();
	void UpdateIndex();
	void ClearAll();
};

// global scope scene
extern Scene scene;