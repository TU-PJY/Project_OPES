#pragma once
#include "Scene.h"

class OtherPlayer : public GameObject {
private:
	FBX idleFBX{};
	FBX moveFBX{};
	FBX shootFBX{};
	FBX deathFBX{};

	XMFLOAT3 position{ -140.0, -1.483, -130.0 };
	XMFLOAT3 positionDest{ -140.0, -1.483, -130.0 };
	XMFLOAT3 rotation{};
	XMFLOAT3 rotationDest{};
	XMFLOAT3 size{ 2.0, 2.0, 2.0 };

	int currentState{ STATE_IDLE };
	int prevState{ STATE_IDLE };
	int renderState{ STATE_IDLE };
	int characterType{};

	AABB frustumAABB{};
	bool inFrustum{};

	bool initState{};

public:
	OtherPlayer(int characterType);
	void updateState();
	void updateAnimation(float Delta);
	void updateBound();
	void updateRenderValue(float Delta);
	void Update(float Delta) override;
	void Render() override;
	void InputPosition(XMFLOAT3& position) override;
	void InputRotation(XMFLOAT3& rotation) override;
	void InputState(unsigned int state) override;
};