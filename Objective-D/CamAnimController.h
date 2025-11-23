#pragma once
#include "Scene.h"
#include "AnimationUtil.h"
#include "TimerUtil.h"
#include "ModePack.h"
#include "CameraController.h"
#include "MathUtil.h"

enum AnimMode {
	MODE_ENTRY,
	MODE_EXIT
};

enum AnimValueType {
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_XMFLOAT
};

// 최상위 레이어에 위치해야함.
class CamAnimController : public GameObject {
private:
	int       animModeFlag{};
	int       currentLevel{};

	SinMove   sinMove{};
	EaseInOut easeInOut{};

	float     currentTime{};

	XMFLOAT3  camRot{};
	XMFLOAT3  camPos{};
	XMFLOAT3  centerPointPos{};
	float     fov{};
	float     fovStart{};
	float     fovEnd{};

	float     screenOpacity{};

	bool      camPosLocated{};

public:
	// 레벨 엔트리/엑시트, 현재 레벨 번호 입력
	CamAnimController(int flag, int level) {
		animModeFlag = flag;
		currentLevel = level;
	}

	void SetCameraStartPosition(const XMFLOAT3& position) override {
		camPos = position;
	}

	void SetCameraFovRange(float start, float end) override {
		fovStart = start;
		fovEnd = end;
	}

	void InputCenterPointPosition(const XMFLOAT3& position) override {
		centerPointPos = position;
	}

	void Update(float delta) override {
		currentTime += delta;

		switch (animModeFlag) {
		case MODE_ENTRY:
			// 애니메이션 모듈에서 값을 업데이트 한 뒤, 카메라에 적용한다.
			// 1초동안 맵 중앙에서 카메라를 한 바퀴 돌려 보여준 후, 중앙 건물 위치를 클로즈업하며 플레이하게 될 위치를 강조한다.
			if (currentTime >= 1.f) {
				if (currentTime <= 5.f)
					sinMove.Update(camRot.y, 0.f, 180.f, 1.f, delta);
				else {
					camPos = XMFLOAT3(centerPointPos.x - 100.f, camPos.y, centerPointPos.z - 100.f);
					camRot = -Math::CalcDegree3D(camPos, centerPointPos);
					easeInOut.Update(fov, fovStart, fovEnd, 1.f, delta);
				}
			}

			camera.Move(camPos);
			camera.Rotate(camRot.x, camRot.y, camRot.z);
			GLOBAL.offsetFOV = fov;

			// 애니메이션이 모두 끝난다면 화면이 어두워지고, 완전히 어두워지면 본 게임으로 진입한다.
			if (currentTime >= 10.f) {
				screenOpacity += delta;

				if (screenOpacity >= 1.f) {
					switch (currentLevel) {
					case 1:
						scene.SwitchMode(Level1::Start);
						break;
					case 2:
						scene.SwitchMode(Level2::Start);
						break;
					case 3:
						scene.SwitchMode(Level3::Start);
						break;
					}
				}
			}
			break;

		case MODE_EXIT:

			break;
		}
	}

	// 화면이 어두워지는 장면 렌더링
	void Render() override {
		BeginRender(RENDER_TYPE_2D_STATIC);
		Transform::Identity(ScaleMatrix);
		Transform::Scale2D(ScaleMatrix, 10.f, 10.f);
		SetColor(XMFLOAT3(0.f, 0.f, 0.f ));
		Render2D(TEX.ColorTex, screenOpacity);
	}
};