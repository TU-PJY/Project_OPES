#pragma once
#include "Gun.h"

class HeavyMachineGun : public Gun {
public:
	HeavyMachineGun(GameObject* Ptr);
	void updateFire(float Delta) override;
	void ReloadGun() override;
	void Render() override;
};
