#pragma once
#include "Gun.h"

class Shotgun : public Gun {
public:
	Shotgun(GameObject* Ptr);
	void Render() override;
	void ReloadGun() override;
	void updateFire(float Delta) override;
	void updateGun(float Delta) override;
};

