#pragma once
#include "DirectX_3D.h"
#include "GameObject.h"

struct RayCastTarget {
	GameObject* ObjectPtr;
	float Distance;
};

class RayTarget {
private:
	std::vector<RayCastTarget> TargetList{};

public:
	void Add(GameObject* ObjectPtr, float Distance);
	void Clear();
	GameObject* GetNearestTarget();
	GameObject* GetFarthestTarget();
};