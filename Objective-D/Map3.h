#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include "ScriptUtil.h"

class Map3 : public GameObject {
private:

	ScriptUtil RockPositionScript{};
	std::vector<ObjectStruct> RockPosition{};

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
		Transform::Move(TranslateMatrix, 0.0, -10.0, 0.0);
		Transform::Rotate(RotateMatrix, 90.0, 0.0, 0.0);
		Transform::Scale(ScaleMatrix, 500.0, 500.0, 1.0);
		Render3D(SysRes.BillboardMesh, TexRes.Magma);

		//for (auto& Rock : RockPosition) {
		//	BeginRender();
		//	Transform::Move(TranslateMatrix, Rock.Position);
		//	Transform::Scale(ScaleMatrix, Rock.Size);
		//	//Transform::Rotate(RotateMatrix, 0.0, Rock.Rotation, 0.0);
		//	Render3D(MeshRes.FloatingRock, TexRes.ColorTex);
		//}


		// 테스트용 플레이어 모델
		BeginRender();
		Transform::Move(TranslateMatrix, -130.0, 0.0, -130.0);
		Transform::Scale(ScaleMatrix, 1.0, 1.0, 1.0);
		Transform::Rotate(RotateMatrix, -90.0, 0.0, 0.0);
		Render3D(MeshRes.TestMesh, TexRes.TestTex);
	}

	void Load() {
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
		}
	}
};