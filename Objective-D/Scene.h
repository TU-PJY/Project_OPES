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
	std::array<std::vector<int>, Layers> DeleteLocation{};
	int CurrentReferPosition{};
	int SceneCommandCount{};
	bool CommandExist{};

	std::string RunningMode{};
	std::deque<GameObject*>* ControlObjectListPtr{};

	Function DestructorBuffer{};
	bool LoopEscapeCommand{};
	
public:
	std::string GetMode();
	void ReleaseDestructor();
	void Init(Function ModeFunction);
	void SwitchMode(Function ModeFunction);
	void SetupMode(std::string ModeName, Function Destructor, std::deque<GameObject*>& ControlObjectList);
	void RegisterControlObjectList(std::deque<GameObject*>& ControlObjectList);
	void InputKeyMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void InputMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void InputMouseMotionMessage(HWND hWnd);
	void Update(float Delta, ID3D12GraphicsCommandList* CmdList);
	void Render();
	GameObject* AddObject(GameObject* Object, std::string Tag, int InputLayer, bool UseController=false);
	GameObject* SearchLayer(int LayerNum, std::string Tag);
	void DeleteObject(GameObject* Object);
	void DeleteObject(std::string Tag, int DeleteRangeFlag);
	GameObject* Find(std::string Tag);
	GameObject* FindMulti(std::string Tag, int Layer, int Index);
	size_t LayerSize(int Layer);
	void CompleteCommand();

	ID3D12RootSignature* CreateObjectShaderSignature(ID3D12Device* Device);
	ID3D12RootSignature* CreateImageShaderSignature(ID3D12Device* Device);
	ID3D12RootSignature* CreateBoundboxShaderSignature(ID3D12Device* Device);
	ID3D12RootSignature* CreateLineShaderSignature(ID3D12Device* Device);
	ID3D12RootSignature* GetGraphicsRootSignature();
	void ReleaseObjects();
	void Exit();

private:
	void CheckHasController(GameObject* Object);
	void AddDeleteLocation(int Layer, int Position);
	void ProcessObjectCommand();
	void ClearAll();
};

// global scope scene
extern Scene scene;