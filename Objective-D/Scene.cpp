#include "Scene.h"
#include "GameResource.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "MouseUtil.h"

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

// 모드 소멸자를 등록한다.
void Scene::RegisterDestructor(Function Destructor) {
	DestructorBuffer = Destructor;
}

// 모드 소멸자 포인터를 끊는다.
void Scene::ReleaseDestructor() {
	DestructorBuffer = nullptr;
}

// 현재 존재하는 모든 객체들을 업데이트하고 렌더링한다.
// 삭제 예약이 활성화된 객체들은 삭제 커맨드 활성화 후 프레임이 끝난 후 일괄 삭제된다.
void Scene::Routine(float FT, ID3D12GraphicsCommandList* CmdList) {
	ObjectCmdList = CmdList;
	for (int i = 0; i < Layers; ++i) {
		for (auto const& Object : ObjectList[i]) {
			Object->Update(FT);
			Object->Render();

			if (Object->DeleteCommand) 
				AddLocation(i, CurrentReferPosition);
			
			++CurrentReferPosition;
		}
		CurrentReferPosition = 0;
	}
}

// 모드를 변경한다. 모드 변경 시 기존 scene에 있던 객체들은 모두 삭제된다.
void Scene::SwitchMode(Function ModeFunction) {
	ClearAll();
	if (DestructorBuffer)
		DestructorBuffer();
	ModeFunction();
}

// 모드 이름을 등록한다. 중복되는 모드 이름을 등록하지 않도록 유의한다.
void Scene::RegisterModeName(std::string ModeName) {
	RunningMode = ModeName;
}

// 컨트롤러 설정 함수이다. 이 함수를 직접 작성할 일은 없다,
void Scene::RegisterKeyController(void (*FunctionPtr)(HWND, UINT, WPARAM, LPARAM)) {
	KeyboardControllerPtr = FunctionPtr;
}
void Scene::RegisterMouseController(void (*FunctionPtr)(HWND, UINT, WPARAM, LPARAM)) {
	MouseControllerPtr = FunctionPtr;
}
void Scene::RegisterMouseMotionController(void (*FunctionPtr)(HWND)) {
	MouseMotionControllerPtr = FunctionPtr;
}

// 객체를 찾아 컨트롤러 함수로 메시지를 전달한다. 다수로 존재하는 객체에 사용하지 않도록 유의한다.
void Scene::InputMouse(std::string ObjectTag, MouseEvent& Event) {
	auto Object = ObjectIndex.find(ObjectTag);
	if (Object != end(ObjectIndex))
		Object->second->InputMouse(Event);
}
void Scene::InputKey(std::string ObjectTag, KeyEvent& Event) {
	auto Object = ObjectIndex.find(ObjectTag);
	if (Object != end(ObjectIndex))
		Object->second->InputKey(Event);
}
void Scene::InputMouseMotion(std::string ObjectTag, MotionEvent& Event) {
	auto Object = ObjectIndex.find(ObjectTag);
	if (Object != end(ObjectIndex))
		Object->second->InputMouseMotion(Event);
}

// 객체를 추가한다. 원하는 객체와 태그, 레이어를 설정할 수 있다.
// 이 함수에서 입력한 태그는 Find()함수에서 사용된다.
void Scene::AddObject(GameObject*&& Object, std::string Tag, int InputLayer) {
	ObjectList[InputLayer].emplace_back(Object);
	ObjectIndex.insert(std::make_pair(Tag, Object));
	Object->ObjectTag = Tag;
	Object->ObjectLayer = InputLayer;
}

// 포인터를 사용하여 객체를 삭제한다. 객체에 삭제 마크를 표시한다.
// 이 코드가 실행되는 시점에 즉시 삭제되지 않음에 유의한다.
// 삭제 마크가 표시된 객체는 UpdateObjectIndex()에서 최종적으로 삭제된다.
// 클래스 내부에서 this 포인터로도 자신을 삭제할 수 있다.
void Scene::DeleteObject(GameObject* Object) {
	Object->DeleteCommand = true;
}

// 현재 존재하는 객체들 중 특정 객체의 포인터를 얻어 접근할 때 사용한다.
// 이진 탐색을 사용하여 검색하므로 매우 빠르다.
GameObject* Scene::Find(std::string Tag) {
	auto Object = ObjectIndex.find(Tag);
	if (Object != std::end(ObjectIndex))
		return Object->second;
	else
		return nullptr;
}

// 특정 태그를 가진 오브젝트들의 포인터 범위를 리턴한다.
// 해당 함수로 equal range를 얻어 for문으로 접근하면 된다.
std::pair<ObjectRange, ObjectRange> Scene::EqualRange(std::string Tag) {
	return ObjectIndex.equal_range(Tag);
}

// 삭제 마크가 표시된 객체를 메모리에서 제거한다.
void Scene::CompleteCommand() {
	if (!CommandExist)
		return;

	ProcessObjectCommand();
	UpdateIndex();
	CommandExist = false;
}

// 프로그램을 종료한다.
void Scene::Exit() {
	PostQuitMessage(1);
}


// 키보드, 마우스, 마우스 움직임을 WinMain으로부터 받아온다. 직접 쓸 일은 없다.
void Scene::InputKeyMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	if (KeyboardControllerPtr)
		KeyboardControllerPtr(hWnd, nMessageID, wParam, lParam);
}
void Scene::InputMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	if (MouseControllerPtr)
		MouseControllerPtr(hWnd, nMessageID, wParam, lParam);
}
void Scene::InputMouseMotionMessage(HWND hWnd) {
	if (MouseMotionControllerPtr)
		MouseMotionControllerPtr(hWnd);
}

// 루트 시그니처를 리턴한다
ID3D12RootSignature* Scene::GetGraphicsRootSignature() {
	return(DefaultRootSignature);
}

// 루트시그니처를 릴리즈한다
void Scene::ReleaseObjects() {
	if (DefaultRootSignature)
		DefaultRootSignature->Release();
}

// 렌더링을 준비한다
void Scene::PrepareRender(ID3D12GraphicsCommandList* CmdList) {
	
}

/////////////// private

// 삭제 위치를 저장한다.
void Scene::AddLocation(int Layer, int Position) {
	DeleteLocation[Layer].emplace_back(Position);
	CommandExist = true;
}

// 삭제 마크가 표시된 객체들을 컨테이너에서 제거한다. 실제로 객체가 삭제되는것이 아님에 유의한다.
void Scene::ProcessObjectCommand() {
	int Offset{};

	for (int Layer = 0; Layer < Layers; ++Layer) {
		size_t Size = DeleteLocation[Layer].size();
		if (Size == 0)
			continue;

		for (int i = 0; i < Size; ++i) {
			auto Object = begin(ObjectList[Layer]) + DeleteLocation[Layer][i] - Offset;
			ObjectList[Layer].erase(Object);
			++SceneCommandCount;
			++Offset;
		}

		DeleteLocation[Layer].clear();
		Offset = 0;
	}
}

// 삭제 마크가 표시된 객체들을 실제로 삭제한다.
void Scene::UpdateIndex() {
	auto Object = begin(ObjectIndex);
	while (Object != end(ObjectIndex) && SceneCommandCount != 0) {
		if (Object->second->DeleteCommand) {
			delete Object->second;
			Object->second = nullptr;
			Object = ObjectIndex.erase(Object);
			--SceneCommandCount;
			continue;
		}
		++Object;
	}
}

// 현재 존재하는 모든 객체들을 삭제한다.
void Scene::ClearAll() {
	for (auto const& Object : ObjectIndex)
		Object.second->DeleteCommand = true;
}
