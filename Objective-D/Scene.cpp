#include "Scene.h"
#include "GameResource.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "MouseUtil.h"

#include "ModePack.h"

// 이 프로젝트의 핵심 유틸이다. 프로그램의 모든 객체의 업데이트 및 렌더링은 모두 이 프레임워크를 거친다.

// 프레임워크를 초기화 한다. 실행 시 단 한 번만 실행되는 함수로, 더미 객체를 추가한 후 모드를 시작한다.
void Scene::Init(Function ModeFunction) {
	// 시작 모드 함수 실행
	ModeFunction();

	// 삭제 위치 버퍼 할당
	for (int i = 0; i < Layers; ++i)
		DeleteLocation[i].reserve(DELETE_LOCATION_BUFFER_SIZE);
}

// 현재 실행 중인 모드 이름을 리턴한다
std::string Scene::GetMode() {
	return RunningMode;
}

// 모드 소멸자 포인터를 끊는다.
void Scene::ReleaseDestructor() {
	DestructorBuffer = nullptr;
}

// 현재 존재하는 모든 객체들을 업데이트하고 렌더링한다.
// 삭제 예약이 활성화된 객체들은 삭제 커맨드 활성화 후 프레임이 끝난 후 일괄 삭제된다.
void Scene::Update(float Delta, ID3D12GraphicsCommandList* CmdList) {
	GlobalCommandList = CmdList;
	for (int L = 0; L < Layers; L++) {
		for (auto const& O : ObjectList[L]) {
			if (!O->DeleteCommand) 
				O->Update(Delta);
			
			// 모드 전환 이벤트가 발생할 경우 곧바로 루프를 건너뛴다
			// 오브젝트 삭제 위치의 무결성을 모장하기 위함
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

// 모드를 변경한다. 모드 변경 시 기존 scene에 있던 객체들은 모두 삭제된다.
void Scene::SwitchMode(Function ModeFunction) {
	ClearAll();
	if (DestructorBuffer)
		DestructorBuffer();
	ModeFunction();
}

// 모드구동에 필요한 데이터들을 Scene에 등록한다.
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

// 객체를 추가한다. 원하는 객체와 태그, 레이어를 설정할 수 있다.
// 이 함수에서 입력한 태그는 Find()함수에서 사용된다.
void Scene::AddObject(GameObject* Object, std::string Tag, int InputLayer, bool UseController) {
	ObjectList[InputLayer].emplace_back(Object);
	Object->ObjectTag = Tag;
	Object->ObjectLayer = InputLayer;
	if (UseController)
		ControlObjectListPtr->emplace_back(Object);
}

// 포인터를 사용하여 객체를 삭제한다. 객체에 삭제 마크를 표시한다.
// 이 코드가 실행되는 시점에 즉시 삭제되지 않음에 유의한다.
// 삭제 마크가 표시된 객체는 UpdateObjectIndex()에서 최종적으로 삭제된다.
// 클래스 내부에서 this 포인터로도 자신을 삭제할 수 있다.
void Scene::DeleteObject(GameObject* Object) {
	Object->DeleteCommand = true;

	// 컨트롤러를 가지고 있을 경우 컨트롤러 목록에서 제거한다.
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

// 현재 존재하는 객체들 중 특정 객체의 포인터를 얻어 접근할 때 사용한다.
// 이진 탐색을 사용하여 검색하므로 매우 빠르다.
GameObject* Scene::Find(std::string Tag) {
	for (int L = 0; L < Layers; L++) {
		for (auto& O : ObjectList[L]) {
			if (O->ObjectTag.compare(Tag) == 0 && !O->DeleteCommand)
				return O;
		}
	}

	return nullptr;
}

// for문을 사용해 특정 레이어에 있는 다수의 동일 객체들에 접근한다.
GameObject* Scene::FindMulti(std::string Tag, int Layer, int Index) {
	if(ObjectList[Layer][Index]->ObjectTag.compare(Tag) == 0 && !ObjectList[Layer][Index]->DeleteCommand)
		return ObjectList[Layer][Index];
	
	return false;
}

// 삭제 마크가 표시된 객체를 메모리에서 제거한다.
void Scene::CompleteCommand() {
	if (!CommandExist)
		return;

	ProcessObjectCommand();
	CommandExist = false;
}

// 프로그램을 종료한다.
void Scene::Exit() {
	PostQuitMessage(1);
}

// 키보드, 마우스, 마우스 움직임을 WinMain으로부터 받아온다. 직접 쓸 일은 없다.
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

// 루트 시그니처를 리턴한다
ID3D12RootSignature* Scene::GetGraphicsRootSignature() {
	return(ObjectShaderRootSignature);
}

// 루트시그니처를 릴리즈한다
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

// 삭제 위치를 저장한다.
void Scene::AddDeleteLocation(int Layer, int Position) {
	DeleteLocation[Layer].emplace_back(Position);
	CommandExist = true;
}

// 삭제 마크가 표시된 객체들을 컨테이너에서 제거한다. 실제로 객체가 삭제되는것이 아님에 유의한다.
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

// 현재 존재하는 모든 객체들을 삭제 대기 상태로 변경한다.
void Scene::ClearAll() {
	for (int L = 0; L < Layers; L++) {
		DeleteLocation[L].clear();
		size_t Size = ObjectList[L].size();

		for (int O = 0; O < Size; O++) {
			ObjectList[L][O]->DeleteCommand = true;
			AddDeleteLocation(L, O);
		}
	}

	// 모션 탭쳐 상태를 강제로 해제한다.
	mouse.EndMotionCapture();

	LoopEscapeCommand = true;
}
