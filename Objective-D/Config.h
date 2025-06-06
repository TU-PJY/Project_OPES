#pragma once
#include <tchar.h>

///////////////////////////////////////////
// �ִϸ��̼� ������ ���� ���
// �� ��带 Ȱ��ȭ �ϸ� ������ Animated FBX���� �ִϸ��̼� ��� ��ȯ �����͸� ������ �� �����Ѵ�.
constexpr bool AnimationDataExtractMode = false;

// �ִϸ��̼� ������ ���� ������
// �����͸� �����ϴ� ������ �����Ѵ�. ���� �������ϼ��� ������ �� �����ɸ����� �� ��ŭ �� �ε巯�� �ִϸ��̼��� ����� �� �ִ�.
constexpr int AnimationExtractFrame = 60;
///////////////////////////////////////////


// ī�޶� ��� ����
// ī�޶� Ŭ���� ��� �Լ� Update(float FT)���� ������ �� ����ϴ� ������ Ŭ�����̴�. ī�޶� Ŭ�������� �ڼ��� �����ϵ��� �Ѵ�.
enum class CamMode{ MODE1 };

// ���α׷� ���� �� ����� ������ ����� �����Ѵ�.
// START_WITH_FULL_SCREEN �ɼ� Ȱ��ȭ �� ����� �ػ󵵷� ����ȴ�.
constexpr int START_UP_WIDTH = 2000;
constexpr int START_UP_HEIGHT = 1000;

// Ǯ��ũ�� ���� ������ ���� �����Ѵ�.
constexpr bool START_WITH_FULL_SCREEN = false;

// ��� ������ ����� Ȱ��ȭ �Ѵ�. ESC�� ������ ��� �����Ѵ�.
constexpr bool DEV_EXIT = true;

// �ִ� ������ ������ �����Ѵ�. 0���� ���� �� �ִ� ���������� �����Ѵ�.
constexpr float FRAME_LIMITS = 0;

// �ٿ��ڽ� ǥ�� ���θ� �����Ѵ�.
//#define SHOW_BOUND_BOX

// ���� ���忡 ����� ���̾ �����Ѵ�. �� �κп��� �ݵ�� EOL(End Of Layer)ǥ�⸦ �ؾ��Ѵ�.
// UI�� ��� ���� �ֻ�� ���̾ �߰��ϴ� ���� �����Ѵ�.
enum LayerEnum
{ LAYER1, LAYER_PARTICLE1, LAYER_PLAYER, LAYER_PARTICLE2, LAYER2, LAYER3, EOL };

// ���� ��ġ ���� ũ�� ����, �⺻�� 200
constexpr int DELETE_LOCATION_BUFFER_SIZE = 200;

// Ȱ��ȭ �� ���� ���� ���� �� �Ȱ� ����� Ȱ��ȭ �ȴ�.
// ��Ȱ��ȭ �� ���� ���� ���� �� �Ȱ� ����� ��Ȱ��ȭ �ȴ�.
constexpr bool ENABLE_FOG_AFTER_BEGIN_RENDER = false;

// Ȱ��ȭ �� 3D ������ ���·� ���� �� �ؽ�ó ���� ������ �ڵ����� �����Ѵ�.
// ��Ȱ��ȭ �� �������� �ʴ´�.
constexpr bool ENABLE_TEXTURE_V_FLIP_AFTER_BEGIN_RENDER = true;

// ����ǥ���� ���ڿ��� �����Ѵ�. �ݵ�� �Ʒ��� ���� �������� �����ؾ� �Ѵ�.
constexpr TCHAR TitleName[] = _T("Objective-D(");