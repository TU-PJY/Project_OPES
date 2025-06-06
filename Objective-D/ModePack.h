#pragma once
#include "Scene.h"
#include "MouseUtil.h"

// 여기에 모드 네임스페이스 함수들을 모아서 선언한다. -> 모드에 사용되는 파일 개수를 줄이기 위함
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

// FBX 애니메이션 구현을 위한 테스트 모드. 실제 개발 시 비활성화한다.
namespace TestMode {
	void Start();
	void Destructor();
}

// 서버 시연용 모드
namespace ServerTestMode {
	void Start();
	void Destructor();
}

