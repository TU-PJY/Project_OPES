#include "StartMode.h"
#include "ModePack.h"

// ��Ʈ�ѷ��� ����� ������Ʈ ������ ����
std::deque<GameObject*> ControlObjectList;

////////////////////////////////////////////////////

// ���۸�带 �̰����� ������ �� �ִ�.
START_MODE_PTR StartMode = Level2::Start;