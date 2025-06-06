#pragma once
#include "CameraUtil.h"
#include "GameObject.h"

namespace PickingUtil {
	//bool PickByCursorFBX(LPARAM lParam, GameObject* Object, FBXMesh& Mesh);
	//bool PickByWinCoordFBX(int X, int Y, GameObject* Object, FBXMesh& Mesh);
	//bool PickByViewportFBX(float X, float Y, GameObject* Object, FBXMesh& Mesh);
	bool PickByCursor(LPARAM lParam, GameObject* Object, Mesh* MeshPtr);
	bool PickByWinCoord(int X, int Y, GameObject* Object, Mesh* MeshPtr);
	bool PickByViewport(float X, float Y, GameObject* Object, Mesh* MeshPtr);
	bool PickByCursorOOBB(LPARAM lParam, const OOBB& Other);
	bool PickByCursorAABB(LPARAM lParam, const AABB& Other);
	bool PickByCursorRange(LPARAM lParam, const BoundSphere& Other);
	bool PickByWinCoordAABB(int X, int Y, const AABB& Other);
	bool PickByWinCoordOOBB(int X, int Y, const OOBB& Other);
	bool PickByWinCoordRange(int X, int Y, const BoundSphere& Other);
	bool PickByViewportAABB(float X, float Y, const AABB& Other);
	bool PickByViewportOOBB(float X, float Y, const OOBB& Other);
	bool PickByViewportRange(float X, float Y, const BoundSphere& Other);
};