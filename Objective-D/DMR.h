#pragma once
#include "Gun.h"

class DMR : public Gun {
public:
	DMR(GameObject* Ptr);
	void Render() override;
	void ReloadGun() override;
	void updateGun(float Delta) override;
	void updateFire(float Delta) override;
};

