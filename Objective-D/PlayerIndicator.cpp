#include "PlayerIndicator.h"

PlayerIndicator::PlayerIndicator() {
	text.SetAlign(ALIGN_DEFAULT);
	text.SetHeightAlign(HEIGHT_DEFAULT);
	text.SetColor(XMFLOAT3(1.0, 1.0, 1.0));
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.01, -0.01), 0.5);
}

void PlayerIndicator::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -1.0 * ASPECT, -1.0);
	Transform::Scale2D(TranslateMatrix, 1.3, 0.32);
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

	text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.88, -1.02 + 0.2), 0.1, "Grenade");

	if (grenCount > 0) {
		text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));
		text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.9, -1.02), 0.2, renderGren);
	}
	else {
		text.SetColor(XMFLOAT3(1.0, 0.0, 0.0));
		text.Render(XMFLOAT2(-1.0 * ASPECT - 0.01 + 0.9, -1.02), 0.2, "X");
	}

	text.SetColor(XMFLOAT3(1.0, 0.9, 0.0));
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
