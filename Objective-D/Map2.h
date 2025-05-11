#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ScriptUtil.h"

#include "Player.h"
#include "CrossHair.h"

class Map2 : public GameObject {
private:
	ScriptUtil WallPositionScript{};
	std::vector<ObjectStruct> WallVec{};

	ScriptUtil ObjectPositionScript{};
	std::vector<ObjectStruct> ObjectVec{};

	ScriptUtil OOBBDataScript{};
	std::vector<OOBB> OOBBVec{};

	TerrainUtil terrain{};

public:
	Map2() {
		Load();
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

				// 시연용 임시 조작키
			case VK_RIGHT:
				scene.SwitchMode(Level3::Start);
				break;

				// 시연용 임시 조작키
			case VK_LEFT:
				scene.SwitchMode(Level1::Start);
				break;

			case VK_DOWN:
				scene.DeleteObject("camera_controller", DELETE_RANGE_ALL);
				scene.AddObject(new CrossHair, "crosshair", LAYER3);
				scene.AddObject(new Player("map2"), "player", LAYER1, true);
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

			switch (Object.Index) {
			case 0: Render3D(MESH.WinterRock[0], TEX.Map2Palette); break;
			case 1: Render3D(MESH.WinterRock[1], TEX.Map2Palette); break;
			case 2: Render3D(MESH.WinterIce[0], TEX.IceTex);       break;
			case 3: Render3D(MESH.WinterIce[1], TEX.IceTex);       break;
			case 4: Render3D(MESH.WinterRock[2], TEX.Map2Palette); break;
			case 5: Render3D(MESH.Mushroom[0], TEX.Palette3);      break;
			}
		}

		for (auto& O : OOBBVec)
			O.Render();
	}

	void Load() {
		WallVec.clear();
		WallPositionScript.Release();
		WallPositionScript.Load("Resources//Scripts//map2//map2-wall-rock.xml");

		// 로드 동작을 람다 함수로 정의한 후 LoadAllData()에 전달하면 LoadAllData()내부에서 정의한 동작을 실행한다.
		auto LoadWallData = [&](CategoryPtr Category) {
			ObjectStruct Obj;
			Obj.Position.x = WallPositionScript.LoadDigitData(Category, "X");
			Obj.Position.y = WallPositionScript.LoadDigitData(Category, "Y");
			Obj.Position.z = WallPositionScript.LoadDigitData(Category, "Z");
			Obj.Size.x = WallPositionScript.LoadDigitData(Category, "SizeX");
			Obj.Size.y = WallPositionScript.LoadDigitData(Category, "SizeY");
			Obj.Size.z = WallPositionScript.LoadDigitData(Category, "SizeZ");
			Obj.Rotation = WallPositionScript.LoadDigitData(Category, "Rotation");

			WallVec.emplace_back(Obj);
		};

		WallPositionScript.LoadAllData(LoadWallData);
		

		ObjectVec.clear();
		ObjectPositionScript.Release();
		ObjectPositionScript.Load("Resources//Scripts//map2//map2-object.xml");

		auto LoadObjectData = [&](CategoryPtr Category) {
			ObjectStruct Obj;
			Obj.Position.x = ObjectPositionScript.LoadDigitData(Category, "X");
			Obj.Position.y = ObjectPositionScript.LoadDigitData(Category, "Y");
			Obj.Position.z = ObjectPositionScript.LoadDigitData(Category, "Z");
			Obj.Size.x = ObjectPositionScript.LoadDigitData(Category, "SizeX");
			Obj.Size.y = ObjectPositionScript.LoadDigitData(Category, "SizeY");
			Obj.Size.z = ObjectPositionScript.LoadDigitData(Category, "SizeZ");
			Obj.Rotation = ObjectPositionScript.LoadDigitData(Category, "Rotation");
			Obj.Index = ObjectPositionScript.LoadDigitData(Category, "Index");

			ObjectVec.emplace_back(Obj);
		};

		ObjectPositionScript.LoadAllData(LoadObjectData);


		OOBBVec.clear();
		OOBBDataScript.Release();
		OOBBDataScript.Load("Resources//Scripts//map2//map2-oobb.xml");

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

	// 맵의 벽 oobb를 얻는다
	std::vector<OOBB> GetMapWallOOBB() {
		return OOBBVec;
	}
};