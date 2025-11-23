#pragma once
#include "Scene.h"
#include "TransformUtil.h"
#include "MathUtil.h"
#include "CameraUtil.h"
#include "RandomUtil.h"

class Spaceship : public GameObject {
public:
	float heightPosNum{};
	float rotationNum{};

	float heightPos{};
	float rotation{};

	Spaceship() {
		heightPosNum = XM_PI / 2.f;
	}

	~Spaceship() {

	}

	void Update(float delta) override {
		heightPosNum += delta * 2.f;
		rotationNum += delta * 2.f;
		heightPos = sin(heightPosNum) * 1.f - 0.5f;
		rotation = sin(rotationNum) * 5.f;
	}

	void Render() override {
		BeginRender();
		Transform::Move(TranslateMatrix, XMFLOAT3(0.f, heightPos, 0.f));
		Transform::Rotate(RotateMatrix, 0.f, 0.f, rotation);
		Render3D(MESH.spaceship, TEX.spaceship);
	}
};