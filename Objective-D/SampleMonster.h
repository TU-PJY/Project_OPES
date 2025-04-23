#pragma once
#include "GameObject.h"
#include "Scene.h"

class SampleMonster : public GameObject {
private:
	XMFLOAT3 position{ -130.0, 20.0, -130.0 };
	XMFLOAT3 rotation{};

	TerrainUtil terr{};

public:
	SampleMonster() {
		//fbxUtil.SelectAnimation(MESH.TestMesh, "mixamo.com");
	}

	void Update(float FrameTime) override {
		UpdateFBXAnimation(MESH.man, FrameTime);
		position.y -= FrameTime * 10.0;

		rotation.y += 50.0 * FrameTime;

		position.x = -130.0 + sin(XMConvertToRadians(rotation.y)) * 3.0;
		position.z = -130.0 + cos(XMConvertToRadians(rotation.y)) * 3.0;

		if (auto terrain = scene.Find("map2"); terrain) {
			terr.InputPosition(position, 0.0);
			if (terr.CheckCollision(terrain->GetTerrain()))
				terr.SetHeightToTerrain(position);
		}
	}

	void Render() override {
		BeginRender();
		SetColor(0.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(RotateMatrix, 0.0, rotation.y + 90.0, 0.0);
		//Transform::Scale(ScaleMatrix, 3.0, 3.0, 3.0);
		//Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		RenderFBX(MESH.man, TEX.man);
	}
};