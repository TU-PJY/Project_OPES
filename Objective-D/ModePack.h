#pragma once
#include "Scene.h"
#include "MouseUtil.h"

// ���⿡ ��� ���ӽ����̽� �Լ����� ��Ƽ� �����Ѵ�. -> ��忡 ���Ǵ� ���� ������ ���̱� ����
namespace Level1 {
	void Start();
	void Destructor();
}

namespace Level2 {
	void Start();
	void Destructor();
}

namespace Level3 {
	void Start();
	void Destructor();
}

// FBX �ִϸ��̼� ������ ���� �׽�Ʈ ���. ���� ���� �� ��Ȱ��ȭ�Ѵ�.
namespace TestMode {
	void Start();
	void Destructor();
}

