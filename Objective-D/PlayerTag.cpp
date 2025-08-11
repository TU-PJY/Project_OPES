#include "PlayerTag.h"
#include "CameraUtil.h"
#include "MathUtil.h"

PlayerTag::PlayerTag(const std::string& name) {
	tag = name;
	tagText.SetAlign(ALIGN_DEFAULT);
	tagText.SetHeightAlign(HEIGHT_DEFAULT);
	tagText.EnableStaticSize();
}

void PlayerTag::Render() {
	Vector vec{};
	float renderMultiply = Math::CalcDistance3D(XMFLOAT3(0.0, 6.0, 0.0), camera.GetPosition()) * 0.5;

	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Math::BillboardLookAt(RotateMatrix, vec, position, camera.GetPosition());

	Transform::Move(TranslateMatrix, 0.0f, 6.0f, 0.0f);
	Math::BillboardLookAt(RotateMatrix, vec, XMFLOAT3(0.0f, 6.0f, 0.0f), camera.GetPosition());
	Transform::Scale(ScaleMatrix, 0.03 * 8.0 * renderMultiply, 0.05 * renderMultiply, 1.0);
	Render3D(SYSRES.BillboardMesh, TEX.ColorTex, 0.6, DEPTH_TEST_NONE);

	tagText.Render3D(XMFLOAT3(position.x, position.y + 6.0, position.z), 0.03, tag);
}

void PlayerTag::InputPosition(XMFLOAT3& Position) {
	position = Position;
}
