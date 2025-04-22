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

	ScriptUtil TreePositionScript{};
	std::vector<ObjectStruct> TreePosition{};

	ScriptUtil StonePositionScript{};
	std::vector<ObjectStruct> StonePosition{};

	ScriptUtil CrystalPositionScript{};
	std::vector<ObjectStruct> CrystalPosition{};

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
		Render3D(SysRes.BillboardMesh, TEX.Magma);

		// 화산
		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		Transform::Scale(ScaleMatrix, 1.5, 1.5, 1.5);
		Transform::Rotate(RotateMatrix, 0.0, -50.0, 0.0);
		Transform::Move(TranslateMatrix, 60.0, 5.0, -60.0);
		Render3D(MESH.Volcano, TEX.Volcano);

		// 분화구
		for (auto& Volcano : VolcanoPosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Volcano.Position);
			Transform::Scale(ScaleMatrix, Volcano.Size);
			Transform::Rotate(RotateMatrix, 0.0, Volcano.Rotation, 0.0);
			Render3D(MESH.SmallVolcano, TEX.Map3Palette);
		}

		// 바닥 돌
		for (auto& Rock : RockPosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Rock.Position);
			Transform::Scale(ScaleMatrix, Rock.Size);
			Transform::Rotate(RotateMatrix, 0.0, Rock.Rotation, 0.0);
			Render3D(MESH.FloatingRock, TEX.Map3Palette);
		}

		// 나무
		for (auto& Tree : TreePosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Tree.Position.x, Tree.Position.y - 1.0, Tree.Position.z);
			Transform::Scale(ScaleMatrix, Tree.Size);
			Transform::Rotate(RotateMatrix, 0.0, Tree.Rotation, 0.0);
			Render3D(MESH.DeadTree, TEX.Map3Palette);
		}

		// 바위
		for (auto& Stone : StonePosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Stone.Position);
			Transform::Scale(ScaleMatrix, Stone.Size);
			Transform::Rotate(RotateMatrix, 0.0, Stone.Rotation, 0.0);
			Render3D(MESH.Map3Stone[Stone.Index], TEX.Map3Palette);
		}

		// 크리스탈
		for (auto& Crystal : CrystalPosition) {
			BeginRender();
			Transform::Move(TranslateMatrix, Crystal.Position);
			Transform::Scale(ScaleMatrix, Crystal.Size);
			Transform::Rotate(RotateMatrix, 0.0, Crystal.Rotation, 0.0);
			Render3D(MESH.Crystal[Crystal.Index], TEX.Map3Palette);
		}

		// 테스트용 플레이어 모델
		BeginRender();
		Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		Render3D(MESH.TestMesh, TEX.TestTex);
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

		TreePosition.clear();
		TreePositionScript.Release();
		TreePositionScript.Load("Resources//Scripts//map3//map3-tree.xml");
		Count = TreePositionScript.GetCategoryNum();
		for (int i = 0; i < Count; i++) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = TreePositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = TreePositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = TreePositionScript.LoadDigitData(CatName, "Z");

			Obj.Size.x = TreePositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = TreePositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = TreePositionScript.LoadDigitData(CatName, "SizeZ");

			Obj.Rotation = TreePositionScript.LoadDigitData(CatName, "Rotation");

			TreePosition.emplace_back(Obj);
		}

		StonePosition.clear();
		StonePositionScript.Release();
		StonePositionScript.Load("Resources//Scripts//map3//map3-stone.xml");
		Count = StonePositionScript.GetCategoryNum();
		for (int i = 0; i < Count; i++) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = StonePositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = StonePositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = StonePositionScript.LoadDigitData(CatName, "Z");

			Obj.Size.x = StonePositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = StonePositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = StonePositionScript.LoadDigitData(CatName, "SizeZ");

			Obj.Rotation = StonePositionScript.LoadDigitData(CatName, "Rotation");

			Obj.Index = StonePositionScript.LoadDigitData(CatName, "Index");

			StonePosition.emplace_back(Obj);
		}

		CrystalPosition.clear();
		CrystalPositionScript.Release();
		CrystalPositionScript.Load("Resources//Scripts//map3//map3-crystal.xml");
		Count = CrystalPositionScript.GetCategoryNum();
		for (int i = 0; i < Count; i++) {
			ObjectStruct Obj{};
			std::string CatName = "Object" + std::to_string(i + 1);
			Obj.Position.x = CrystalPositionScript.LoadDigitData(CatName, "X");
			Obj.Position.y = CrystalPositionScript.LoadDigitData(CatName, "Y");
			Obj.Position.z = CrystalPositionScript.LoadDigitData(CatName, "Z");

			Obj.Size.x = CrystalPositionScript.LoadDigitData(CatName, "SizeX");
			Obj.Size.y = CrystalPositionScript.LoadDigitData(CatName, "SizeY");
			Obj.Size.z = CrystalPositionScript.LoadDigitData(CatName, "SizeZ");

			Obj.Rotation = CrystalPositionScript.LoadDigitData(CatName, "Rotation");

			Obj.Index = CrystalPositionScript.LoadDigitData(CatName, "Index");

			CrystalPosition.emplace_back(Obj);
		} 
	}
};