#pragma once
#include "Scene.h"

// 모든 총이 상속받는 부모 클래스
// 총 객체 생성 시 spec부분만 재정의해서 사용한다.
class Gun : public GameObject {
protected:
	// spec
	float   recoil         {};
	int     damage         {};
	int     totalAmmo      {};
	int     currentAmmo    {};
	float   flameRenderTime{};
	float   fireDelayTime  {};
	float   reloadTime     {};
	//
	
	// state
	XMFLOAT3 position{};
	XMFLOAT3 positionOffset{};
	float    recoilOffset{};

	XMFLOAT3 rotation{};
	XMFLOAT3 rotationDest{};

	bool     moveState{};
	bool     zoomState{};
	bool     triggerState{};
	bool     reloadState{};
	bool     fireEnableState{};
	float    currentFireDelayTime{};
	float    reloadDelayTime{};
	float    currentFlameRenderTime{};

	// animation
	float shakeValueX{};
	float shakeValueY{};
	float shakeResultX{};
	float shakeResultY{};
	float shakeRatio{};

	// 사용자 객체의 포인터, 주로 Player1st
	GameObject* userPtr{};

public:
	void enableZoom() override;
	void disableZoom() override;
	void pullTrigger() override;
	void releaseTrigger() override;
	void InputPosition(XMFLOAT3& position) override;
	void inputRotation(const XMFLOAT3& rotation) override;

	void inputMoveState(bool moveState) override;
	int getTotalAmmo() override;
	int getCurrentAmmo() override;

	void updateGun(float Delta);
	void updateFire(float Delta);
	void updateAnimation(float Delta);
	void Update(float Delta) override;
};