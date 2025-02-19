#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ScriptUtil.h"

class Map3 : public GameObject {
private:

	ScriptUtil RockPositionScript{};
	std::vector<ObjectStruct> RockPosition{};

	ScriptUtil VolcanoPositionScript{};
	std::vector<ObjectStruct> VolcanoPosition{};

public:
	Map3() {
		Load();
	}

	void InputKey(KeyEvent& Event) override {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case 'R':
				Load();
				break;
			}
		}
	}

	void Update(float FrameTime) override {

	}

	void Render() override {
		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		Transform::Move(TranslateMatrix, 0.0, -30.0, 0.0);
		Transform::Rotate(RotateMatrix, 90.0, 0.0, 0.0);
		Transform::Scale(ScaleMatrix, 1000.0, 1000.0, 1.0);
		Render3D(SysRes.BillboardMesh, TexRes.Magma);

		// 화산
		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		Transform::Scale(ScaleMatrix, 1.5, 1.5, 1.5);
		Transform::Rotate(RotateMatrix, 0.0, -50.0, 0.0);
		Transform::Move(TranslateMatrix, 60.0, 5.0, -60.0);
		Render3D(MeshRes.Volcano, TexRes.Volcano);

		// 분화구
		for (auto& Volcano : VolcanoPosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Volcano.Position);
			Transform::Scale(ScaleMatrix, Volcano.Size);
			Transform::Rotate(RotateMatrix, 0.0, Volcano.Rotation, 0.0);
			Render3D(MeshRes.SmallVolcano, TexRes.Map3Palette);
		}

		for (auto& Rock : RockPosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Rock.Position);
			Transform::Scale(ScaleMatrix, Rock.Size);
			Transform::Rotate(RotateMatrix, 0.0, Rock.Rotation, 0.0);
			Render3D(MeshRes.FloatingRock, TexRes.Map3Palette);
		}


		// 테스트용 플레이어 모델
		BeginRender();
		Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		Render3D(MeshRes.TestMesh, TexRes.TestTex);
	}

	void Load() {
		RockPosition.clear();
		RockPositionScript.Release();
		RockPositionScript.Load("Resources//Scripts//map3//map3-rock.xml");
		int Count = RockPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; i++) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = RockPositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = RockPositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = RockPositionScript.LoadDigitData(CatName, "Z");

			Obj.Size.x = RockPositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = RockPositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = RockPositionScript.LoadDigitData(CatName, "SizeZ");

			Obj.Rotation = RockPositionScript.LoadDigitData(CatName, "Rotation");

			RockPosition.emplace_back(Obj);
		}

		VolcanoPosition.clear();
		VolcanoPositionScript.Release();
		VolcanoPositionScript.Load("Resources//Scripts//map3//map3-volcano.xml");
		Count = VolcanoPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; i++) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = VolcanoPositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = VolcanoPositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = VolcanoPositionScript.LoadDigitData(CatName, "Z");

			Obj.Size.x = VolcanoPositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = VolcanoPositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = VolcanoPositionScript.LoadDigitData(CatName, "SizeZ");

			Obj.Rotation = VolcanoPositionScript.LoadDigitData(CatName, "Rotation");

			VolcanoPosition.emplace_back(Obj);
		}
	}
};