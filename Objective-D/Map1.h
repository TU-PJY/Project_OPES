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
	std::vector<RockStruct> ObjectPosition{};
	ScriptUtil PositionScript{};

	std::vector<RockStruct> LakeObjectPosition{};
	ScriptUtil LakePositionScript{};

public:
	Map1() {
		ObjectPosition.clear();
		PositionScript.Release();
		PositionScript.Load("Resources//Scripts//map1-wall-rock.xml");
		int Count = PositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			RockStruct rockStruct{};
			rockStruct.Position.x = PositionScript.LoadDigitData(Str, "X");
			rockStruct.Position.y = PositionScript.LoadDigitData(Str, "Y");
			rockStruct.Position.z = PositionScript.LoadDigitData(Str, "Z");
		    rockStruct.Size.x = PositionScript.LoadDigitData(Str, "SizeX");
			rockStruct.Size.y = PositionScript.LoadDigitData(Str, "SizeY");
			rockStruct.Size.z = PositionScript.LoadDigitData(Str, "SizeZ");
			rockStruct.Rotation = PositionScript.LoadDigitData(Str, "Rotation");

			ObjectPosition.emplace_back(rockStruct);
		}

		LakeObjectPosition.clear();
		LakePositionScript.Release();
		LakePositionScript.Load("Resources//Scripts//map1-lake-rock.xml");
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
	}

	void InputKey(KeyEvent& Event) override {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case 'R':
			{
				ObjectPosition.clear();
				PositionScript.Release();
				PositionScript.Load("Resources//Scripts//map1-wall-rock.xml");

				int Count = PositionScript.GetCategoryNum();
				for (int i = 0; i < Count; ++i) {
					std::string Str = "Object";
					Str += std::to_string(i + 1);
					RockStruct rockStruct{};
					rockStruct.Position.x = PositionScript.LoadDigitData(Str, "X");
					rockStruct.Position.y = PositionScript.LoadDigitData(Str, "Y");
					rockStruct.Position.z = PositionScript.LoadDigitData(Str, "Z");
					rockStruct.Size.x = PositionScript.LoadDigitData(Str, "SizeX");
					rockStruct.Size.y = PositionScript.LoadDigitData(Str, "SizeY");
					rockStruct.Size.z = PositionScript.LoadDigitData(Str, "SizeZ");
					rockStruct.Rotation = PositionScript.LoadDigitData(Str, "Rotation");

					ObjectPosition.emplace_back(rockStruct);
				}

				LakeObjectPosition.clear();
				LakePositionScript.Release();
				LakePositionScript.Load("Resources//Scripts//map1-lake-rock.xml");
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
			}
			break;
			}
		}
	}

	void Render() override {
		// 맵 플레인 렌더링
		BeginRender();
		//Transform::Rotate(RotateMatrix, 0.0, 0.0, 0.0);
		//SetColor(0.0, 0.7, 0.0);
		Transform::Move(TranslateMatrix, 0.0, -50.0, 0.0);
		Transform::Scale(ScaleMatrix, 0.3, 0.2, 0.3);
		Render3D(MeshRes.TerrainMesh1, TexRes.Palette1);

		{
			// 호수 렌더링
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 0.0, -1.5, 0.0);
			Transform::Scale(ScaleMatrix, 0.01, 0.01, 0.01);
			Render3D(MeshRes.LakeMesh, TexRes.Palette2);

			// 중앙 호숫바위 렌더링
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 12.5, -4.0, -2.0);
			Transform::Scale(ScaleMatrix, 0.03, 0.04, 0.03);
			Render3D(MeshRes.RockMesh, TexRes.Palette1);

			// 물 렌더링
			BeginRender();
			SetColor(0.05, 0.53, 0.80);
			Transform::Rotate(RotateMatrix, 90.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, 6.0, 0.6, 8.0);
			Transform::Scale(ScaleMatrix, 41.0, 40.0, 1.0);
			Render3D(SysRes.BillboardMesh, TexRes.ColorTex, 0.5);

			// 호수 주변 바위 렌더링
			for (auto& P : LakeObjectPosition) {
				BeginRender();
				SetColor(0.0, 0.0, 0.0);
				Transform::Move(TranslateMatrix, P.Position);
				Transform::Scale(ScaleMatrix, P.Size);
				Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
				Render3D(MeshRes.LakeRockMesh[P.Index], TexRes.Palette2);
			}
		}

		// 테스트용 플레이어 모델
		BeginRender();
		SetColor(0.0, 0.0, 0.0);
		Transform::Move(TranslateMatrix, -130.0 ,0.0, -130.0);
		Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		Render3D(MeshRes.TestMesh, TexRes.TestTex);


		for (auto& P : ObjectPosition) {
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P.Position);
			Transform::Scale(ScaleMatrix, P.Size);
			Transform::Rotate(RotateMatrix, 0.0, P.Rotation, 0.0);
			Render3D(MeshRes.RockMesh, TexRes.Palette1);
		}
	}
};