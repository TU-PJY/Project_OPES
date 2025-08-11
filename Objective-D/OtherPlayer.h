#pragma once
#include "Scene.h"
#include "MeshUtil.h"
#include "TextUtil.h"

class OtherPlayer : public GameObject {
private:
	//FBX playerFBX[4]{};
	
	FBX idleFBX{};
	FBX moveFBX{};
	FBX shootFBX{};
	FBX deathFBX{};

	QP::Sound shootSound{};

	XMFLOAT3 position{ -140.0, -1.483, -130.0 };
	XMFLOAT3 positionDest{ -140.0, -1.483, -130.0 };
	XMFLOAT3 rotation{};
	XMFLOAT3 rotationDest{};
	XMFLOAT3 size{ 2.0, 2.0, 2.0 };

	OOBB     playerBound{};

	int currentState{ STATE_IDLE };
	int prevState{ -1 };
	int renderState{ STATE_IDLE };
	int characterType{};

	BoundSphere frustumBound{};
	bool inFrustum{};

	int totalHP{};
	int currentHP{};

	unsigned int ID{};

	float flameRenderTime{};

	float flameIdlex{};
	float flameIdley{};
	float flameIdlez{};

	float flameMovex{};
	float flameMovey{};
	float flameMovez{};

	float footstepTime{};
	float footstepInterval{ 0.5 };

	std::string playerName{};
	GameObject* tagObject{};

public:
	OtherPlayer(int characterType, unsigned int ID, const std::string name);
	void updateState();
	void updateAnimation(float Delta);
	void updateBound();
	void updateRenderValue(float Delta);
	void updateDeath();
	void Update(float Delta) override;
	void Render() override;
	void RenderIdleFlame();
	void RenderMoveFlame();
	void InputPosition(XMFLOAT3& position) override;
	void InputRotation(XMFLOAT3& rotation) override;
	void InputState(unsigned int state) override;
	XMFLOAT3 GetPosition() override;
	void InputHP(int currentHP) override;
	OOBB GetOOBB() override;
	void GiveDamage(int damage) override;
	unsigned int GetID() override;

	void addFlameTime() override;
};