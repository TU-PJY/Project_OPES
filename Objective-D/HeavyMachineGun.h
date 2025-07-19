#pragma once
#include "Gun.h"

class HeavyMachineGun : public Gun {
public:
	HeavyMachineGun(GameObject* Ptr);
	void Render() override;
};
