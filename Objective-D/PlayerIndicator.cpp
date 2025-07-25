#include "PlayerIndicator.h"

PlayerIndicator::PlayerIndicator(int characterType) {
	text.SetAlign(ALIGN_DEFAULT);
	text.SetHeightAlign(HEIGHT_DEFAULT);
	text.SetColor(XMFLOAT3(1.0, 1.0, 1.0));
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.01, -0.01), 0.5);

	type = characterType;

	if (type == CHARACTER_ENG)
		currentIndex = 3;
	else
		currentIndex = 1;

	text2.SetAlign(ALIGN_MIDDLE);
	text2.SetColor(xmfloat3(1.0, 1.0, 1.0));
	text2.EnableShadow();
	text2.SetShadow(xmfloat2(0.005, -0.005), 0.6);
}

void PlayerIndicator::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.125 + 0.25 * currentIndex, -1.0 + 0.45);
	Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
	SetColor(1.0, 1.0, 1.0);
	Render2D(TEX.ColorTex, 0.8);

	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT, -1.0);
	Transform::Scale2D(TranslateMatrix, 0.8, 0.32);
	Transform::Move2D(TranslateMatrix, 0.5, 0.5);
	Render2D(TEX.ColorTex, 0.3);

	std::string renderHP = std::to_string(currentHP);
	std::string renderAmmo = std::to_string(currentAmmo);
	std::string renderGren = std::to_string(grenCount);

	//////////////////////////////////////// HP

	text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.08, -1.02 + 0.2), 0.1, "HP");

	if (currentHP > totalHP / 4)
		text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));
	else
		text.SetColor(XMFLOAT3(1.0, 0.0, 0.0));

	text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.1, -1.02), 0.2, renderHP);

	text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));

	//////////////////////////////////////// Ammo

	text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.48, -1.02 + 0.2), 0.1, "Ammo");

	if (currentAmmo > 0) {
		text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));
		text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.5, -1.02), 0.2, renderAmmo);
	}
	else {
		text.SetColor(XMFLOAT3(1.0, 0.0, 0.0));
		text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.5, -1.02), 0.2, "R");
	}

	text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));

	//////////////////////////////////////// Greneade

	

	//if (type != CHARACTER_ENG) {
	//	text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.88, -1.02 + 0.2), 0.1, "Grenade");

	//	if (grenCount > 0) {
	//		text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));
	//		text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.9, -1.02), 0.2, renderGren);
	//	}
	//	else {
	//		text.SetColor(XMFLOAT3(1.0, 0.0, 0.0));
	//		text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.9, -1.02), 0.2, "X");
	//	}

	//	text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));
	//}

	if (type == CHARACTER_ENG) {
		std::string turretCoolTimeStr = std::to_string(turretCoolTime);
		std::string beaconCoolTimeStr = std::to_string(beaconCoolTime);
		std::string barrierCoolTimeStr = std::to_string(barrierCoolTime);

		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.125, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
		if(turretCoolTime == 0)
			Render2D(TEX.UI_turretIcon);
		else {
			Render2D(TEX.UI_turretIcon, 0.5);
			text2.Render(xmfloat2(-1.0 * ASPECT + 0.125, -1.0 + 0.6), 0.1, turretCoolTimeStr);
		}

		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.375, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
		if(beaconCoolTime == 0)
			Render2D(TEX.UI_beaconIcon);
		else {
			Render2D(TEX.UI_beaconIcon, 0.5);
			text2.Render(xmfloat2(-1.0 * ASPECT + 0.375, -1.0 + 0.6), 0.1, beaconCoolTimeStr);
		}

		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.625, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);

		if (GLOBAL.DefenseState) {
			if(barrierCoolTime == 0)
				Render2D(TEX.UI_barrierIcon);
			else {
				Render2D(TEX.UI_barrierIcon, 0.5);
				text2.Render(xmfloat2(-1.0 * ASPECT + 0.625, -1.0 + 0.6), 0.1, barrierCoolTimeStr);
			}
		}
		else
			Render2D(TEX.UI_barrierIcon, 0.5);
	}

	else {
		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.125, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
		Render2D(TEX.UI_grenadeIcon);
		text2.Render(xmfloat2(-1.0 * ASPECT + 0.125, -1.0 + 0.6), 0.1, std::to_string(grenCount));
	}

	switch (type) {
	case CHARACTER_MG:
		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.125 + 0.25, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
		Render2D(TEX.UI_mgIcon);
		break;

	case CHARACTER_DMR:
		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.125 + 0.25, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
		Render2D(TEX.UI_dmrIcon);
		break;

	case CHARACTER_ENG:
		BeginRender(RENDER_TYPE_2D);
		Transform::Move2D(TranslateMatrix, -1.0 * ASPECT + 0.875, -1.0 + 0.45);
		Transform::Scale2D(ScaleMatrix, 0.25, 0.25);
		Render2D(TEX.UI_shotgunIcon);
		break;
	}
}

void PlayerIndicator::InputHP(int total, int current) {
	totalHP = total;
	currentHP = current;
}

void PlayerIndicator::InputAmmo(int fullAmmo, int currentAmmo) {
	totalAmmo = fullAmmo;
	this->currentAmmo = currentAmmo;
}

void PlayerIndicator::InputGrenade(int value) {
	grenCount = value;
}

void PlayerIndicator::InputTurretCoolTime(float time) {
	turretCoolTime = (int)time;
}

void PlayerIndicator::InputBeaconCoolTime(float time) {
	beaconCoolTime = (int)time;
}

void PlayerIndicator::InputBarrierCoolTime(float time) {
	barrierCoolTime = (int)time;
}

void PlayerIndicator::ScrollRight() {
	currentIndex++;
	if (type == CHARACTER_ENG && currentIndex > 3)
		currentIndex = 0;

	else if (type != CHARACTER_ENG && currentIndex > 1)
		currentIndex = 0;
}

void PlayerIndicator::ScrollLeft() {
	currentIndex--;
	if (currentIndex < 0) {
		if (type == CHARACTER_ENG)
			currentIndex = 3;
		else
			currentIndex = 1;
	}
}

int PlayerIndicator::GetCurrentIndex() {
	return currentIndex;
}
