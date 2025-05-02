#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ScriptUtil.h"

class Map2 : public GameObject {
private:
	ScriptUtil WallPositionScript{};
	std::vector<ObjectStruct> WallVec{};

	ScriptUtil ObjectPositionScript{};
	std::vector<ObjectStruct> ObjectVec{};

	TerrainUtil terrain{};

public:
	Map2() {
		Load();
		//BeginRender();
		Transform::Identity(TranslateMatrix);
		Transform::Identity(ScaleMatrix);
		Transform::Identity(RotateMatrix);

		Transform::Move(TranslateMatrix, 0.0, -50.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.3, 0.2, 0.3);
		terrain.InputData(TranslateMatrix, RotateMatrix, ScaleMatrix, MESH.TerrainMesh1);
	}

	TerrainUtil GetTerrain() override {
		return terrain;
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
		Render3D(MESH.TerrainMesh1, TEX.Map2TerrainTex);

		for (auto& Wall : WallVec) {
			BeginRender();
			SetColor(0.2, 0.2, 0.2);
			Transform::Move(TranslateMatrix, Wall.Position.x, Wall.Position.y - 4.0, Wall.Position.z);
			Transform::Scale(ScaleMatrix, Wall.Size);
			Transform::Rotate(RotateMatrix, -90.0, 0.0, Wall.Rotation);
			Render3D(MESH.WinterWall, TEX.Map2Palette);
		}

		for (auto& Object : ObjectVec) {
			BeginRender();
			Transform::Move(TranslateMatrix, Object.Position);
			Transform::Scale(ScaleMatrix, Object.Size);

			if(Object.Index == 0 || Object.Index == 1 || Object.Index == 4)
				Transform::Rotate(RotateMatrix, -90.0, 0.0, Object.Rotation);
			else
				Transform::Rotate(RotateMatrix, 0.0, Object.Rotation, 0.0);

			if (Object.Index == 0)
				Render3D(MESH.WinterRock[0], TEX.Map2Palette);
			else if (Object.Index == 1)
				Render3D(MESH.WinterRock[1], TEX.Map2Palette);
			else if (Object.Index == 2)
				Render3D(MESH.WinterIce[0], TEX.IceTex);
			else if (Object.Index == 3)
				Render3D(MESH.WinterIce[1], TEX.IceTex);
			else if (Object.Index == 4)
				Render3D(MESH.WinterRock[2], TEX.Map2Palette);
			else if (Object.Index == 5)
				Render3D(MESH.Mushroom[0], TEX.Palette3);
		}

		//// 테스트용 플레이어 모델
		//BeginRender();
		//SetColor(0.0, 0.0, 0.0);
		//Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		//Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		//Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		//Render3D(MESH.TestMesh, TEX.TestTex);
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

		ObjectVec.clear();
		ObjectPositionScript.Release();
		ObjectPositionScript.Load("Resources//Scripts//map2//map2-object.xml");
		Count = ObjectPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = ObjectPositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = ObjectPositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = ObjectPositionScript.LoadDigitData(CatName, "Z");
			Obj.Size.x = ObjectPositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = ObjectPositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = ObjectPositionScript.LoadDigitData(CatName, "SizeZ");
			Obj.Rotation = ObjectPositionScript.LoadDigitData(CatName, "Rotation");
			Obj.Index = ObjectPositionScript.LoadDigitData(CatName, "Index");

			ObjectVec.emplace_back(Obj);
		}
	}
};