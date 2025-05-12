#include "Scene.h"
#include "GameResource.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "MouseUtil.h"

#include "ModePack.h"

// �� ������Ʈ�� �ٽ� ��ƿ�̴�. ���α׷��� ��� ��ü�� ������Ʈ �� �������� ��� �� �����ӿ�ũ�� ��ģ��.

// �����ӿ�ũ�� �ʱ�ȭ �Ѵ�. ���� �� �� �� ���� ����Ǵ� �Լ���, ���� ��ü�� �߰��� �� ��带 �����Ѵ�.
void Scene::Init(Function ModeFunction) {
	// ���� ��� �Լ� ����
	ModeFunction();

	// ���� ��ġ ���� �Ҵ�
	for (int i = 0; i < Layers; ++i)
		DeleteLocation[i].reserve(DELETE_LOCATION_BUFFER_SIZE);
}

// ���� ���� ���� ��� �̸��� �����Ѵ�
std::string Scene::GetMode() {
	return RunningMode;
}

// ��� �Ҹ��� �����͸� ���´�.
void Scene::ReleaseDestructor() {
	DestructorBuffer = nullptr;
}

// ���� �����ϴ� ��� ��ü���� ������Ʈ�ϰ� �������Ѵ�.
// ���� ������ Ȱ��ȭ�� ��ü���� ���� Ŀ�ǵ� Ȱ��ȭ �� �������� ���� �� �ϰ� �����ȴ�.
void Scene::Update(float Delta, ID3D12GraphicsCommandList* CmdList) {
	GlobalCommandList = CmdList;
	for (int L = 0; L < Layers; L++) {
		for (auto const& O : ObjectList[L]) {
			if (!O->DeleteCommand) 
				O->Update(Delta);
			
			// ��� ��ȯ �̺�Ʈ�� �߻��� ��� ��ٷ� ������ �ǳʶڴ�
			// ������Ʈ ���� ��ġ�� ���Ἲ�� �����ϱ� ����
			if (LoopEscapeCommand) {
				LoopEscapeCommand = false;
				CurrentReferPosition = 0;
				return;
			}

			if (O->DeleteCommand)
				AddDeleteLocation(L, CurrentReferPosition);

			CurrentReferPosition++;
		}
		CurrentReferPosition = 0;
	}
}

void Scene::Render() {
	for (int L = 0; L < Layers; L++) {
		for (auto const& O : ObjectList[L]) {
			if (!O->DeleteCommand)
				O->Render();
		}
	}
}

// ��带 �����Ѵ�. ��� ���� �� ���� scene�� �ִ� ��ü���� ��� �����ȴ�.
void Scene::SwitchMode(Function ModeFunction) {
	ClearAll();
	if (DestructorBuffer)
		DestructorBuffer();
	ModeFunction();
}

// ��屸���� �ʿ��� �����͵��� Scene�� ����Ѵ�.
void Scene::SetupMode(std::string ModeName, Function Destructor, std::deque<GameObject*>& ControlObjectList) {
	RunningMode = ModeName;
	DestructorBuffer = Destructor;
	ControlObjectList.clear();
	ControlObjectListPtr = &ControlObjectList;

	std::cout << "Mode Switched to [ " << ModeName << " ] Mode.\n";
}

void Scene::RegisterControlObjectList(std::deque<GameObject*>& ControlObjectList) {
	ControlObjectListPtr = &ControlObjectList;
}

// ��ü�� �߰��Ѵ�. ���ϴ� ��ü�� �±�, ���̾ ������ �� �ִ�.
// �� �Լ����� �Է��� �±״� Find()�Լ����� ���ȴ�.
void Scene::AddObject(GameObject* Object, std::string Tag, int InputLayer, bool UseController) {
	ObjectList[InputLayer].emplace_back(Object);
	Object->ObjectTag = Tag;
	Object->ObjectLayer = InputLayer;
	if (UseController)
		ControlObjectListPtr->emplace_back(Object);
}

// �����͸� ����Ͽ� ��ü�� �����Ѵ�. ��ü�� ���� ��ũ�� ǥ���Ѵ�.
// �� �ڵ尡 ����Ǵ� ������ ��� �������� ������ �����Ѵ�.
// ���� ��ũ�� ǥ�õ� ��ü�� UpdateObjectIndex()���� ���������� �����ȴ�.
// Ŭ���� ���ο��� this �����ͷε� �ڽ��� ������ �� �ִ�.
void Scene::DeleteObject(GameObject* Object) {
	Object->DeleteCommand = true;

	// ��Ʈ�ѷ��� ������ ���� ��� ��Ʈ�ѷ� ��Ͽ��� �����Ѵ�.
	CheckHasController(Object);
}

void Scene::DeleteObject(std::string Tag, int DeleteRangeFlag) {
	if (DeleteRangeFlag == DELETE_RANGE_SINGLE) {
		if (auto Found = Find(Tag); Found)
			DeleteObject(Found);
	}

	else if (DeleteRangeFlag == DELETE_RANGE_ALL) {
		for (int L = 0; L < Layers; L++) {
			size_t Size = ObjectList[L].size();
			for (int O = 0; O < Size; O++) {
				if (auto Found = FindMulti(Tag, L, O); Found)
					DeleteObject(Found);
			}
		}
	}
}

// ���� �����ϴ� ��ü�� �� Ư�� ��ü�� �����͸� ��� ������ �� ����Ѵ�.
// ���� Ž���� ����Ͽ� �˻��ϹǷ� �ſ� ������.
GameObject* Scene::Find(std::string Tag) {
	for (int L = 0; L < Layers; L++) {
		for (auto& O : ObjectList[L]) {
			if (O->ObjectTag.compare(Tag) == 0 && !O->DeleteCommand)
				return O;
		}
	}

	return nullptr;
}

// for���� ����� Ư�� ���̾ �ִ� �ټ��� ���� ��ü�鿡 �����Ѵ�.
GameObject* Scene::FindMulti(std::string Tag, int Layer, int Index) {
	if(ObjectList[Layer][Index]->ObjectTag.compare(Tag) == 0 && !ObjectList[Layer][Index]->DeleteCommand)
		return ObjectList[Layer][Index];
	
	return false;
}

// ���� ��ũ�� ǥ�õ� ��ü�� �޸𸮿��� �����Ѵ�.
void Scene::CompleteCommand() {
	if (!CommandExist)
		return;

	ProcessObjectCommand();
	CommandExist = false;
}

// ���α׷��� �����Ѵ�.
void Scene::Exit() {
	PostQuitMessage(1);
}

// Ű����, ���콺, ���콺 �������� WinMain���κ��� �޾ƿ´�. ���� �� ���� ����.
void Scene::InputKeyMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	KeyEvent Event{ hWnd, nMessageID, wParam, lParam };
	if (DEV_EXIT && Event.Type == WM_KEYDOWN && Event.Key == VK_ESCAPE)
		scene.Exit();
	for (auto const& C : *ControlObjectListPtr)
		if (C && !C->DeleteCommand) C->InputKey(Event);
}

void Scene::InputMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	MouseEvent Event{ hWnd, nMessageID, wParam, lParam };
	for (auto const& C : *ControlObjectListPtr)
		if (C && !C->DeleteCommand) C->InputMouse(Event);
}

void Scene::InputMouseMotionMessage(HWND hWnd) {
	MotionEvent Event{ hWnd, mouse.MotionPosition };
	mouse.UpdateMousePosition(hWnd);
	for (auto const& C : *ControlObjectListPtr)
		if (C && !C->DeleteCommand) C->InputMouseMotion(Event);
}

// ��Ʈ �ñ״�ó�� �����Ѵ�
ID3D12RootSignature* Scene::GetGraphicsRootSignature() {
	return(ObjectShaderRootSignature);
}

// ��Ʈ�ñ״�ó�� �������Ѵ�
void Scene::ReleaseObjects() {
	if (ObjectShaderRootSignature)
		ObjectShaderRootSignature->Release();
}

/////////////// private

void Scene::CheckHasController(GameObject* Object) {
	auto Found = std::find(ControlObjectListPtr->begin(), ControlObjectListPtr->end(), Object);
	if (Found != ControlObjectListPtr->end())
		ControlObjectListPtr->erase(Found);
}

// ���� ��ġ�� �����Ѵ�.
void Scene::AddDeleteLocation(int Layer, int Position) {
	DeleteLocation[Layer].emplace_back(Position);
	CommandExist = true;
}

// ���� ��ũ�� ǥ�õ� ��ü���� �����̳ʿ��� �����Ѵ�. ������ ��ü�� �����Ǵ°��� �ƴԿ� �����Ѵ�.
void Scene::ProcessObjectCommand() {
	int Offset{};

	for (int L = 0; L < Layers; ++L) {
		size_t Size = DeleteLocation[L].size();
		if (Size == 0)
			continue;

		for (int O = 0; O < Size; ++O) {
			auto Object = begin(ObjectList[L]) + DeleteLocation[L][O] - Offset;
			*Object = nullptr;
			Object = ObjectList[L].erase(Object);
			++SceneCommandCount;
			++Offset;
		}

		DeleteLocation[L].clear();
		Offset = 0;
	}
}

// ���� �����ϴ� ��� ��ü���� ���� ��� ���·� �����Ѵ�.
void Scene::ClearAll() {
	for (int L = 0; L < Layers; L++) {
		DeleteLocation[L].clear();
		size_t Size = ObjectList[L].size();

		for (int O = 0; O < Size; O++) {
			ObjectList[L][O]->DeleteCommand = true;
			AddDeleteLocation(L, O);
		}
	}

	// ��� ���� ���¸� ������ �����Ѵ�.
	mouse.EndMotionCapture();

	LoopEscapeCommand = true;
}
