#include "Explosion.h"

Explosion::Explosion(const XMFLOAT3& createPosition) {
	position = createPosition;
	fbx.SetSpeed(1.0);
}

void Explosion::Update(float Delta) {
	fbx.UpdateAnimation(Delta);
	if (fbx.GetAnimationEndState())
		scene.DeleteObject(this);
}

void Explosion::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);

	SetColor(1.0, 0.8, 0.0);
	RenderFBX(fbx, TEX.ColorTex);

	for (int i = 0; i < 3; i++) {
		SetColor(1.0, 0.0, 0.0);
		Transform::Rotate(RotateMatrix, 35.0, 35.0, 35.0);
		RenderFBX(fbx, TEX.ColorTex);

		SetColor(1.0, 0.5, 0.0);
		Transform::Rotate(RotateMatrix, 35.0, 35.0, 35.0);
		RenderFBX(fbx, TEX.ColorTex);

		SetColor(1.0, 1.0, 0.0);
		Transform::Rotate(RotateMatrix, 35.0, 35.0, 35.0);
		RenderFBX(fbx, TEX.ColorTex);
	}
}
