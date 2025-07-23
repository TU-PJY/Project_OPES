#pragma once
#include "Gun.h"

class Shotgun : public Gun {
public:
	Shotgun(GameObject* Ptr);
	void Render() override;
};

