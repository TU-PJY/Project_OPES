#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ScriptUtil.h"

class Map2 : public GameObject {
private:
	ScriptUtil WallPositionScript{};
	std::vector<ObjectStruct> WallVec{};

public:
	Map2() {
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

	void Render() override {
		// 맵 터레인 렌더링
		BeginRender();
		Transform::Move(TranslateMatrix, 0.0, -50.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.3, 0.2, 0.3);
		//SetColor(0.8, 0.8, 0.8);
		Render3D(MeshRes.TerrainMesh1, TexRes.Palette1);

		for (auto& Wall : WallVec) {
			BeginRender();
			SetColor(0.2, 0.2, 0.2);
			Transform::Move(TranslateMatrix, Wall.Position);
			Transform::Scale(ScaleMatrix, Wall.Size);
			Transform::Rotate(RotateMatrix, -90.0, 0.0, Wall.Rotation);
			Render3D(MeshRes.WinterRock, TexRes.Map2Palette);
		}

		// 테스트용 플레이어 모델
		BeginRender();
		SetColor(0.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		Render3D(MeshRes.TestMesh, TexRes.TestTex);
	}

	void Load() {
		WallVec.clear();
		WallPositionScript.Release();
		WallPositionScript.Load("Resources//Scripts//map2//map2-wall-rock.xml");
		int Count = WallPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = WallPositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = WallPositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = WallPositionScript.LoadDigitData(CatName, "Z");
			Obj.Size.x = WallPositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = WallPositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = WallPositionScript.LoadDigitData(CatName, "SizeZ");
			Obj.Rotation = WallPositionScript.LoadDigitData(CatName, "Rotation");

			WallVec.emplace_back(Obj);
		}
	}
};