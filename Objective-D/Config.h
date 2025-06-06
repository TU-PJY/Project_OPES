#pragma once
#include <tchar.h>

///////////////////////////////////////////
// 애니메이션 데이터 추출 모드
// 이 모드를 활성화 하면 지정한 Animated FBX에서 애니메이션 행렬 변환 데이터를 추출한 후 종료한다.
constexpr bool AnimationDataExtractMode = false;

// 애니메이션 데이터 추출 프레임
// 데이터를 추출하는 간격을 설정한다. 높은 프레임일수록 추출이 더 오래걸리지만 그 만큼 더 부드러운 애니메이션을 사용할 수 있다.
constexpr int AnimationExtractFrame = 60;
///////////////////////////////////////////


// 카메라 모드 설정
// 카메라 클래스 멤버 함수 Update(float FT)에서 실행할 때 사용하는 열거형 클래스이다. 카메라 클래스에서 자세히 설명하도록 한다.
enum class CamMode{ MODE1 };

// 프로그램 시작 시 사용할 윈도우 사이즈를 설정한다.
// START_WITH_FULL_SCREEN 옵션 활성화 시 모니터 해상도로 실행된다.
constexpr int START_UP_WIDTH = 2000;
constexpr int START_UP_HEIGHT = 1000;

// 풀스크린 모드로 시작할 지를 설정한다.
constexpr bool START_WITH_FULL_SCREEN = false;

// 즉시 나가기 기능을 활성화 한다. ESC를 누르면 즉시 종료한다.
constexpr bool DEV_EXIT = true;

// 최대 프레임 제한을 설정한다. 0으로 설정 시 최대 프레임으로 동작한다.
constexpr float FRAME_LIMITS = 0;

// 바운드박스 표시 여부를 설정한다.
//#define SHOW_BOUND_BOX

// 게임 월드에 사용할 레이어를 설정한다. 끝 부분에는 반드시 EOL(End Of Layer)표기를 해야한다.
// UI의 경우 가장 최상단 레이어에 추가하는 것을 권장한다.
enum LayerEnum
{ LAYER1, LAYER_PARTICLE1, LAYER_PLAYER, LAYER_PARTICLE2, LAYER2, LAYER3, EOL };

// 삭제 위치 버퍼 크기 지정, 기본값 200
constexpr int DELETE_LOCATION_BUFFER_SIZE = 200;

// 활성화 시 렌더 상태 설정 시 안개 사용이 활성화 된다.
// 비활성화 시 렌더 상태 설정 시 안개 사용이 비활성화 된다.
constexpr bool ENABLE_FOG_AFTER_BEGIN_RENDER = false;

// 활성화 시 3D 렌더링 상태로 설정 시 텍스처 수직 반전을 자동으로 적용한다.
// 비활성화 시 적용하지 않는다.
constexpr bool ENABLE_TEXTURE_V_FLIP_AFTER_BEGIN_RENDER = true;

// 제목표시줄 문자열을 설정한다. 반드시 아래와 같은 형식으로 저정해야 한다.
constexpr TCHAR TitleName[] = _T("Objective-D(");