#include "CrossHair.h"


CrossHair::CrossHair() {
	crosshair.SetColor(1.0, 0.0, 0.0);
}

void CrossHair::InputMouse(MouseEvent& Event) {
	if (Event.Type == WM_RBUTTONDOWN)
		render_state = false;
	else if (Event.Type == WM_RBUTTONUP)
		render_state = true;
}

// 플레이어 총 발사 시 반동 값 부여
void CrossHair::InputRecoil(float Value) {
	recoil += Value;
}

void CrossHair::Update(float Delta) {
	// 반동은 항상 0.0을 향해 선형 보간한다
	recoil = std::lerp(recoil, 0.0, Delta * 5.0);
}

void CrossHair::EnableRender() {
	render_state = true;
}

void CrossHair::DisableRender() {
	render_state = false;
}

void CrossHair::Render() {
	if (!render_state)
		return;

	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 0.1, 0.1);
	Render2D(TEX.UI_crosshair);
}