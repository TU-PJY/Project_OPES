#pragma once
#include "Scene.h"

class Scope : public GameObject{
private:
	bool renderState{};
	float size{ 5.0 };

public:
	void Render() override;
	void Update(float Delta) override;
	void SetRenderState(bool flag) override;
	void SetSize(float Value) override;
};

