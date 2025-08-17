#include "PlayerTag.h"
#include "CameraUtil.h"
#include "MathUtil.h"

PlayerTag::PlayerTag(const std::string& name) {
	tag = name;
	tagText.SetAlign(ALIGN_MIDDLE);
	tagText.SetHeightAlign(HEIGHT_MIDDLE);
	tagText.EnableStaticSize();
	tagText.SetColor(XMFLOAT3(1.0, 1.0, 1.0));
}

void PlayerTag::SetBuffDebuff(unsigned int ID) {
	for (int i = 0; i < 5; i++) {
		if (i > 0) {
			if (GLOBAL.playerList[ID].buff[i])
				buffState[i] = true;
			if (GLOBAL.playerList[ID].deBuff[i])
				debuffState[i] = true;
		}
	}
}

void PlayerTag::Render() {
	Vector vec{};
	XMFLOAT3 renderPosition = XMFLOAT3(position.x, position.y + 4.0, position.z);
	float renderMultiply = Math::CalcDistance3D(renderPosition, camera.GetPosition()) * 0.5;

	XMFLOAT4X4 rotMat{};

	BeginRender();
	SetFogUse(DISABLE_FOG);
	SetLightUse(DISABLE_LIGHT);
	Transform::Move(TranslateMatrix, renderPosition);
	Math::BillboardLookAt(RotateMatrix, vec, renderPosition, camera.GetPosition());
	rotMat = RotateMatrix;
	Transform::Scale(ScaleMatrix, 0.03 * 8.0 * renderMultiply, 0.05 * renderMultiply, 1.0);
	Render3D(SYSRES.BillboardMesh, TEX.ColorTex, 0.6, DEPTH_TEST_NONE);

	tagText.Render3D(renderPosition, 0.03, tag);

	float renderOffset{};
	for (int i = 1; i < 3; i++) {
		if (buffState[i]) {
			BeginRender();
			SetFogUse(DISABLE_FOG);
			SetLightUse(DISABLE_LIGHT);
			Transform::Move(TranslateMatrix, renderPosition);
			Math::BillboardLookAt(RotateMatrix, vec, renderPosition, camera.GetPosition());
			Transform::Scale(ScaleMatrix, 0.07 * renderMultiply, 0.07 * renderMultiply, 1.0);
			Transform::Move(ScaleMatrix, -2.3 + 1.5 * renderOffset, 1.1, 0.0);
			Render3D(SYSRES.BillboardMesh, TEX.UI_buff[i], 1.0, DEPTH_TEST_NONE);
		}

		if (debuffState[i]) {
			BeginRender();
			SetFogUse(DISABLE_FOG);
			SetLightUse(DISABLE_LIGHT);
			Transform::Move(TranslateMatrix, renderPosition);
			Math::BillboardLookAt(RotateMatrix, vec, renderPosition, camera.GetPosition());
			Transform::Scale(ScaleMatrix, 0.07 * renderMultiply, 0.07 * renderMultiply, 1.0);
			Transform::Move(ScaleMatrix, (2.3 - 1.5) + 1.5 * renderOffset, 1.1, 0.0);
			Render3D(SYSRES.BillboardMesh, TEX.UI_deBuff[i], 1.0, DEPTH_TEST_NONE);
		}
		renderOffset++;
	}
}

void PlayerTag::InputPosition(XMFLOAT3& Position) {
	position = Position;
}
