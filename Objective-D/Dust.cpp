#include "Dust.h"
#include "CameraUtil.h"
#include "MathUtil.h"

Dust::Dust(const xmfloat3& createPosition, const xmfloat3& createSize, const xmfloat3& destSize, const xmfloat3& createColor) {
	position = createPosition;
	size = createSize;
	sizeDest = destSize;
	color = createColor;
	currentPlayTime = 5.0;
}

void Dust::Update(float Delta) {
	currentPlayTime += Delta * 10.0;
	currentFrame = (int)currentPlayTime;
	opacity -= Delta * 0.5;
	Math::LerpXMFLOAT3(size, sizeDest, 4.0, Delta);

	if (currentFrame > 24)
		scene.DeleteObject(this);
}

void Dust::Render() {
	Vector vec{};
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Math::BillboardLookAt(RotateMatrix, vec, position, camera.GetPosition());
	Transform::Scale(ScaleMatrix, size);
	SetColor(color);
	Render3D(SYSRES.BillboardMesh, TEX.smoke[currentFrame], opacity);
}
