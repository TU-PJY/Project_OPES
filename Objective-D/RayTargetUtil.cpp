#include "RayTargetUtil.h"

void RayTarget::Add(GameObject* ObjectPtr, float Distance) {
	RayCastTarget NewTarget = { ObjectPtr, Distance };
	TargetList.emplace_back(NewTarget);
}

void RayTarget::Clear() {
	if (TargetList.empty())
		return;

	TargetList.clear();
}

GameObject* RayTarget::GetNearestTarget() {
	if (TargetList.empty())
		return nullptr;

	auto NearestDistance = std::min_element(TargetList.begin(), TargetList.end(),
		[](const RayCastTarget& a, const RayCastTarget& b) {
		return a.Distance < b.Distance;
	});
	
	if (NearestDistance != TargetList.end())
		return NearestDistance->ObjectPtr;

	return nullptr;
}

GameObject* RayTarget::GetFarthestTarget() {
	if (TargetList.empty())
		return nullptr;

	auto FarthestDistance = std::max_element(TargetList.begin(), TargetList.end(),
		[](const RayCastTarget& a, const RayCastTarget& b) {
		return a.Distance > b.Distance;
	});

	if (FarthestDistance != TargetList.end())
		return FarthestDistance->ObjectPtr;

	return nullptr;
}
