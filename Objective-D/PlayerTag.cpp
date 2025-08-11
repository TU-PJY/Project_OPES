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
	XMFLOAT3 renderPosition = XMFLOAT3(position.x, position.y + 6.0, position.z);
	float renderMultiply = Math::CalcDistance3D(renderPosition, camera.GetPosition()) * 0.5;

	BeginRender();
	Transform::Move(TranslateMatrix, renderPosition);
	Math::BillboardLookAt(RotateMatrix, vec, renderPosition, camera.GetPosition());
	Transform::Scale(ScaleMatrix, 0.03 * 8.0 * renderMultiply, 0.05 * renderMultiply, 1.0);
	Render3D(SYSRES.BillboardMesh, TEX.ColorTex, 0.6, DEPTH_TEST_NONE);

	tagText.Render3D(renderPosition, 0.03, tag);
}

void PlayerTag::InputPosition(XMFLOAT3& Position) {
	position = Position;
}
