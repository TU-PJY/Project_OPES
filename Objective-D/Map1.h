#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ScriptUtil.h"

class Map1 : public GameObject {
private:
	std::vector<ObjectStruct> WallObjectPosition{};
	ScriptUtil WallPositionScript{};

	std::vector<ObjectStruct> LakeObjectPosition{};
	ScriptUtil LakePositionScript{};

	std::vector<ObjectStruct> MapObjectPosition{};
	ScriptUtil MapObjectPositionScript{};

	std::vector<ObjectStruct> SmallObjectPosition{};
	ScriptUtil SmallObjectPositionScript{};

	ScriptUtil OOBBDataScript{};
	std::vector<OOBB> OOBBVec{};

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
		Render3D(MESH.TerrainMesh1, TEX.Palette1);

		// È£¼ö ·»´õ¸µ
		{
			// È£¼ö ÁöÇü
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 0.0, -1.8, 0.0);
			Transform::Rotate(TranslateMatrix, 0.0, 180.0, 0.0);
			Transform::Scale(ScaleMatrix, 0.01, 0.01, 0.01);
			Render3D(MESH.LakeMesh, TEX.Palette2);

			// Áß¾Ó È£¼ý¹ÙÀ§ ·»´õ¸µ
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 12.5, -2.0, -4.0);
			Transform::Scale(ScaleMatrix, 0.03, 0.04, 0.03);
			Render3D(MESH.RockMesh, TEX.Palette1);

			// ¹° ·»´õ¸µ
			BeginRender();
			SetColor(0.05, 0.53, 0.80);
			Transform::Rotate(RotateMatrix, 90.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 9.0, 0.6, -7.0);
			Transform::Scale(ScaleMatrix, 55.0, 41.0, 1.0);
			Render3D(SysRes.BillboardMesh, TEX.ColorTex, 0.5);

			// È£¼ö ÁÖº¯ ¹ÙÀ§ ·»´õ¸µ
			for (auto& P : LakeObjectPosition) {
				BeginRender();
				SetColor(0.0, 0.0, 0.0);
				Transform::Move(TranslateMatrix, P.Position);
				Transform::Scale(ScaleMatrix, P.Size);
				Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
				Render3D(MESH.LakeRockMesh[P.Index], TEX.Palette2);
			}
		}

		// ¸Ê ¸è ·»´õ¸µ
		for (auto& P : WallObjectPosition) {
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P.Position.x, P.Position.y - 5.0, P.Position.z);
			Transform::Scale(ScaleMatrix, P.Size);
			Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
			Render3D(MESH.RockMesh, TEX.Palette1);
		}

		// ¸Ê ¿ÀºêÁ§Æ® ·»´õ¸µ
		for (auto& P : MapObjectPosition) {
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P.Position);
			Transform::Scale(ScaleMatrix, P.Size);
			Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
			Render3D(MESH.MapObjectMesh[P.Index], TEX.Palette3);
		}

		// ¼ÒÇü ¸Ê ¿ÀºêÁ§Æ® ·»´õ¸µ
		for (auto& P : SmallObjectPosition) {
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P.Position);
			Transform::Scale(ScaleMatrix, P.Size);
			Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
			// 0: Grass1
			// 1: Grass2
			// 2: Flower1
			// 3: Flower2
			// 4: Mushroom1
			// 5: MushRoom2
			switch (P.Index) {
			case 0:
				Render3D(MESH.Grass[0], TEX.Palette3);
				break;
			case 1:
				Render3D(MESH.Grass[1], TEX.Palette3);
				break;
			case 2:
				Render3D(MESH.Flower[0], TEX.Palette3);
				break;
			case 3:
				Render3D(MESH.Flower[1], TEX.Palette3);
				break;
			case 4:
				Render3D(MESH.Mushroom[0], TEX.Palette3);
				break;
			case 5:
				Render3D(MESH.Mushroom[1], TEX.Palette3);
				break;
			}
		}

		// oobb ·»´õ¸µ
		for (auto& O : OOBBVec)
			O.Render();

		//// Å×½ºÆ®¿ë ÇÃ·¹ÀÌ¾î ¸ðµ¨
		//BeginRender();
		//SetColor(0.0, 0.0, 0.0);
		//Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		//Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		//Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		//Render3D(MESH.TestMesh, TEX.TestTex);
	}

	void Load() {
		WallObjectPosition.clear();
		WallPositionScript.Release();
		WallPositionScript.Load("Resources//Scripts//map1//map1-wall-rock.xml");

		int Count = WallPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			ObjectStruct objectStruct{};
			objectStruct.Position.x = WallPositionScript.LoadDigitData(Str, "X");
			objectStruct.Position.y = WallPositionScript.LoadDigitData(Str, "Y");
			objectStruct.Position.z = WallPositionScript.LoadDigitData(Str, "Z");
			objectStruct.Size.x = WallPositionScript.LoadDigitData(Str, "SizeX");
			objectStruct.Size.y = WallPositionScript.LoadDigitData(Str, "SizeY");
			objectStruct.Size.z = WallPositionScript.LoadDigitData(Str, "SizeZ");
			objectStruct.Rotation = WallPositionScript.LoadDigitData(Str, "Rotation");

			WallObjectPosition.emplace_back(objectStruct);
		}

		LakeObjectPosition.clear();
		LakePositionScript.Release();
		LakePositionScript.Load("Resources//Scripts//map1//map1-lake-rock.xml");
		Count = LakePositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			ObjectStruct objectStruct{};
			objectStruct.Position.x = LakePositionScript.LoadDigitData(Str, "X");
			objectStruct.Position.y = LakePositionScript.LoadDigitData(Str, "Y");
			objectStruct.Position.z = LakePositionScript.LoadDigitData(Str, "Z");
			objectStruct.Size.x = LakePositionScript.LoadDigitData(Str, "SizeX");
			objectStruct.Size.y = LakePositionScript.LoadDigitData(Str, "SizeY");
			objectStruct.Size.z = LakePositionScript.LoadDigitData(Str, "SizeZ");
			objectStruct.Rotation = LakePositionScript.LoadDigitData(Str, "Rotation");
			objectStruct.Index = LakePositionScript.LoadDigitData(Str, "Index");

			LakeObjectPosition.emplace_back(objectStruct);
		}

		MapObjectPosition.clear();
		MapObjectPositionScript.Release();
		MapObjectPositionScript.Load("Resources//Scripts//map1//map1-object.xml");
		Count = MapObjectPositionScript.GetCategoryNum();

		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			ObjectStruct objectStruct{};
			objectStruct.Position.x = MapObjectPositionScript.LoadDigitData(Str, "X");
			objectStruct.Position.y = MapObjectPositionScript.LoadDigitData(Str, "Y");
			objectStruct.Position.z = MapObjectPositionScript.LoadDigitData(Str, "Z");
			objectStruct.Size.x = MapObjectPositionScript.LoadDigitData(Str, "SizeX");
			objectStruct.Size.y = MapObjectPositionScript.LoadDigitData(Str, "SizeY");
			objectStruct.Size.z = MapObjectPositionScript.LoadDigitData(Str, "SizeZ");
			objectStruct.Rotation = MapObjectPositionScript.LoadDigitData(Str, "Rotation");
			objectStruct.Index = MapObjectPositionScript.LoadDigitData(Str, "Index");

			MapObjectPosition.emplace_back(objectStruct);
		}

		SmallObjectPosition.clear();
		SmallObjectPositionScript.Release();
		SmallObjectPositionScript.Load("Resources//Scripts//map1//map1-object-small.xml");
		Count = SmallObjectPositionScript.GetCategoryNum();

		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			ObjectStruct rockStruct{};
			rockStruct.Position.x = SmallObjectPositionScript.LoadDigitData(Str, "X");
			rockStruct.Position.y = SmallObjectPositionScript.LoadDigitData(Str, "Y");
			rockStruct.Position.z = SmallObjectPositionScript.LoadDigitData(Str, "Z");
			rockStruct.Size.x = SmallObjectPositionScript.LoadDigitData(Str, "SizeX");
			rockStruct.Size.y = SmallObjectPositionScript.LoadDigitData(Str, "SizeY");
			rockStruct.Size.z = SmallObjectPositionScript.LoadDigitData(Str, "SizeZ");
			rockStruct.Rotation = SmallObjectPositionScript.LoadDigitData(Str, "Rotation");
			rockStruct.Index = SmallObjectPositionScript.LoadDigitData(Str, "Index");

			SmallObjectPosition.emplace_back(rockStruct);
		}

		OOBBVec.clear();
		OOBBDataScript.Release();
		OOBBDataScript.Load("Resources//Scripts//map1//map1-oobb.xml");

		auto LoadOOBBData = [&](CategoryPtr Category) {
			OOBB oobb;
			XMFLOAT3 Position;
			XMFLOAT3 Size;
			float Degree;

			Position.y = 3.0;
			Position.x = OOBBDataScript.GetDigitData(Category, "X");
			Position.z = OOBBDataScript.GetDigitData(Category, "Z");
			Size.y = 20.0;
			Size.x = OOBBDataScript.GetDigitData(Category, "SizeX");
			Size.z = OOBBDataScript.GetDigitData(Category, "SizeZ");
			Degree = OOBBDataScript.GetDigitData(Category, "Rotation");

			oobb.Update(Position, Size, XMFLOAT3(0.0, Degree, 0.0));
			OOBBVec.emplace_back(oobb);
			};

		OOBBDataScript.LoadAllData(LoadOOBBData);
	}

	// ¸ÊÀÇ º® oobb¸¦ ¾ò´Â´Ù
	std::vector<OOBB> GetMapWallOOBB() {
		return OOBBVec;
	}
};