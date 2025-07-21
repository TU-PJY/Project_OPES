#pragma once
#include "Scene.h"

class Explosion : public GameObject {
private:
	XMFLOAT3 position{};
	FBX fbx{ MESH.explosion };

public:
	Explosion(const XMFLOAT3& createPosition);
	void Update(float Delta) override;
	void Render() override;
};

