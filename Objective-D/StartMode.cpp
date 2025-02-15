#include "StartMode.h"
#include "ModePack.h"

// 컨트롤러에 사용할 오브젝트 포인터 벡터
std::vector<GameObject*> ControlObjectList;

////////////////////////////////////////////////////

// 시작모드를 이곳에서 설정할 수 있다.
START_MODE_PTR StartMode = Level2::Start;