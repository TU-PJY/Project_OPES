#pragma once
#include "CameraUtil.h"
#include "GameObject.h"

namespace PickingUtil {
	bool PickByCursor(LPARAM lParam, GameObject* Object, Mesh* MeshPtr);
	bool PickByWinCoord(int X, int Y, GameObject* Object, Mesh* MeshPtr);
	bool PickByViewport(float X, float Y, GameObject* Object, Mesh* MeshPtr);
	bool PickByCursorOOBB(LPARAM lParam, const OOBB& Other);
	bool PickByCursorAABB(LPARAM lParam, const AABB& Other);
	bool PickByCursorRange(LPARAM lParam, const Range& Other);
	bool PickByWinCoordAABB(int X, int Y, const AABB& Other);
	bool PickByWinCoordOOBB(int X, int Y, const OOBB& Other);
	bool PickByWinCoordRange(int X, int Y, const Range& Other);
	bool PickByViewportAABB(float X, float Y, const AABB& Other);
	bool PickByViewportOOBB(float X, float Y, const OOBB& Other);
	bool PickByViewportRange(float X, float Y, const Range& Other);
};