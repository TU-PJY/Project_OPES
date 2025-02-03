#pragma once
#include "GameObject.h"
#include "MouseUtil.h"
#include "CameraUtil.h"
#include <cmath>
#include "PickingUtil.h"
#include "ScriptUtil.h"

class Map1 : public GameObject {
private:
	std::vector<XMFLOAT3> ObjectPosition{};
	ScriptUtil PositionScript{};

public:
	Map1() {
		ObjectPosition.clear();
		PositionScript.Release();
		PositionScript.Load("Resources//Scripts//Map1.xml");
		int Count = PositionScript.GetCategoryNum();
		for (int i = 0; i < Count; ++i) {
			std::string Str = "Object";
			Str += std::to_string(i + 1);
			XMFLOAT3 LoadedPosition{};
			LoadedPosition.x = PositionScript.LoadDigitData(Str, "X");
			LoadedPosition.y = PositionScript.LoadDigitData(Str, "Y");
			LoadedPosition.z = PositionScript.LoadDigitData(Str, "Z");

			ObjectPosition.emplace_back(LoadedPosition);
		}
	}

	void InputKey(KeyEvent& Event) override {
		if (Event.Type == WM_KEYDOWN) {
			switch (Event.Key) {
			case 'R':
			{
				ObjectPosition.clear();
				PositionScript.Release();
				PositionScript.Load("Resources//Scripts//Map1.xml");

				int Count = PositionScript.GetCategoryNum();
				for (int i = 0; i < Count; ++i) {
					std::string Str = "Object";
					Str += std::to_string(i + 1);
					XMFLOAT3 LoadedPosition{};
					LoadedPosition.x = PositionScript.LoadDigitData(Str, "X");
					LoadedPosition.y = PositionScript.LoadDigitData(Str, "Y");
					LoadedPosition.z = PositionScript.LoadDigitData(Str, "Z");

					ObjectPosition.emplace_back(LoadedPosition);
				}
			}
			break;
			}
		}
	}

	void Render() override {
		// ¸Ê ÇÃ·¹ÀÎ ·»´õ¸µ
		BeginRender();
		SetColor(0.0, 0.8, 0.0);
		Transform::Rotate(RotateMatrix, 90.0, 0.0, 0.0);
		Transform::Scale(ScaleMatrix, 60.0, 60.0, 1.0);
		Render3D(SysRes.BillboardMesh, TexRes.ColorTex);


		for (auto& P : ObjectPosition) {
			BeginRender();
			BeginRender();
			SetColor(0.0, 0.0, 0.0);
			Transform::Move(TranslateMatrix, P);
			Transform::Scale(ScaleMatrix, 0.1, 0.1, 0.1);
			Render3D(MeshRes.RockMesh, TexRes.RockTex);
		}
	}
};