#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include <cmath>
#include "PickingUtil.h"
#include "ScriptUtil.h"

struct RockStruct {
	XMFLOAT3 Position;
	XMFLOAT3 Size;
	float Rotation;
	int Index;
};

class Map1 : public GameObject {
private:
	std::vector<RockStruct> WallObjectPosition{};
	ScriptUtil WallPositionScript{};

	std::vector<RockStruct> LakeObjectPosition{};
	ScriptUtil LakePositionScript{};

	std::vector<RockStruct> MapObjectPosition{};
	ScriptUtil MapObjectPositionScript{};

public:
	Map1() {
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
		// ¸Ê ÅÍ·¹ÀÎ ·»´õ¸µ
		BeginRender();
		Transform::Move(TranslateMatrix, 0.0, -50.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.3, 0.2, 0.3);
		Render3D(MeshRes.TerrainMesh1, TexRes.Palette1);

		// È£¼ö ·»´õ¸µ
		{
			// È£¼ö ÁöÇü
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 0.0, -1.5, 0.0);
			Transform::Scale(ScaleMatrix, 0.01, 0.01, 0.01);
			Render3D(MeshRes.LakeMesh, TexRes.Palette2);

			// Áß¾Ó È£¼ý¹ÙÀ§ ·»´õ¸µ
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 12.5, -4.0, -2.0);
			Transform::Scale(ScaleMatrix, 0.03, 0.04, 0.03);
			Render3D(MeshRes.RockMesh, TexRes.Palette1);

			// ¹° ·»´õ¸µ
			BeginRender();
			SetColor(0.05, 0.53, 0.80);
			Transform::Rotate(RotateMatrix, 90.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 6.0, 0.6, 8.0);
			Transform::Scale(ScaleMatrix, 41.0, 40.0, 1.0);
			Render3D(SysRes.BillboardMesh, TexRes.ColorTex, 0.5);

			// È£¼ö ÁÖº¯ ¹ÙÀ§ ·»´õ¸µ
			for (auto& P : LakeObjectPosition) {
				BeginRender();
				SetColor(0.0, 0.0, 0.0);
				Transform::Move(TranslateMatrix, P.Position);
				Transform::Scale(ScaleMatrix, P.Size);
				Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
				Render3D(MeshRes.LakeRockMesh[P.Index], TexRes.Palette2);
			}
		}

		// ¸Ê ¸è ·»´õ¸µ
		for (auto& P : WallObjectPosition) {
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P.Position);
			Transform::Scale(ScaleMatrix, P.Size);
			Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
			Render3D(MeshRes.RockMesh, TexRes.Palette1);
		}

		// ¸Ê ¿ÀºêÁ§Æ® ·»´õ¸µ
		for (auto& P : MapObjectPosition) {
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P.Position);
			Transform::Scale(ScaleMatrix, P.Size);
			Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
			Render3D(MeshRes.MapObjectMesh[P.Index], TexRes.Palette3);
		}

		// Å×½ºÆ®¿ë ÇÃ·¹ÀÌ¾î ¸ðµ¨
		BeginRender();
		SetColor(0.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		Render3D(MeshRes.TestMesh, TexRes.TestTex);
	}

	void Load() {
		WallObjectPosition.clear();
		WallPositionScript.Release();
		WallPositionScript.Load("Resources//Scripts//map1//map1-wall-rock.xml");

		int Count = WallPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			RockStruct rockStruct{};
			rockStruct.Position.x = WallPositionScript.LoadDigitData(Str, "X");
			rockStruct.Position.y = WallPositionScript.LoadDigitData(Str, "Y");
			rockStruct.Position.z = WallPositionScript.LoadDigitData(Str, "Z");
			rockStruct.Size.x = WallPositionScript.LoadDigitData(Str, "SizeX");
			rockStruct.Size.y = WallPositionScript.LoadDigitData(Str, "SizeY");
			rockStruct.Size.z = WallPositionScript.LoadDigitData(Str, "SizeZ");
			rockStruct.Rotation = WallPositionScript.LoadDigitData(Str, "Rotation");

			WallObjectPosition.emplace_back(rockStruct);
		}

		LakeObjectPosition.clear();
		LakePositionScript.Release();
		LakePositionScript.Load("Resources//Scripts//map1//map1-lake-rock.xml");
		Count = LakePositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			RockStruct rockStruct{};
			rockStruct.Position.x = LakePositionScript.LoadDigitData(Str, "X");
			rockStruct.Position.y = LakePositionScript.LoadDigitData(Str, "Y");
			rockStruct.Position.z = LakePositionScript.LoadDigitData(Str, "Z");
			rockStruct.Size.x = LakePositionScript.LoadDigitData(Str, "SizeX");
			rockStruct.Size.y = LakePositionScript.LoadDigitData(Str, "SizeY");
			rockStruct.Size.z = LakePositionScript.LoadDigitData(Str, "SizeZ");
			rockStruct.Rotation = LakePositionScript.LoadDigitData(Str, "Rotation");
			rockStruct.Index = LakePositionScript.LoadDigitData(Str, "Index");

			LakeObjectPosition.emplace_back(rockStruct);
		}

		MapObjectPosition.clear();
		MapObjectPositionScript.Release();
		MapObjectPositionScript.Load("Resources//Scripts//map1//map1-object.xml");
		Count = MapObjectPositionScript.GetCategoryNum();

		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			RockStruct rockStruct{};
			rockStruct.Position.x = MapObjectPositionScript.LoadDigitData(Str, "X");
			rockStruct.Position.y = MapObjectPositionScript.LoadDigitData(Str, "Y");
			rockStruct.Position.z = MapObjectPositionScript.LoadDigitData(Str, "Z");
			rockStruct.Size.x = MapObjectPositionScript.LoadDigitData(Str, "SizeX");
			rockStruct.Size.y = MapObjectPositionScript.LoadDigitData(Str, "SizeY");
			rockStruct.Size.z = MapObjectPositionScript.LoadDigitData(Str, "SizeZ");
			rockStruct.Rotation = MapObjectPositionScript.LoadDigitData(Str, "Rotation");
			rockStruct.Index = MapObjectPositionScript.LoadDigitData(Str, "Index");

			MapObjectPosition.emplace_back(rockStruct);
		}
	}
};