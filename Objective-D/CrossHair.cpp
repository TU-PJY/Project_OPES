#include "CrossHair.h"


CrossHair::CrossHair() {
	crosshair.SetColor(1.0, 0.0, 0.0);
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

	// 크로스헤어 렌더링
	// 반동 값이 높을 수록 크로스 헤어의 간격은 넓어지게 된다 -> 정확도가 떨어지게 된다
	//  왼쪽 가로 선
	crosshair.Draw(-0.1 - recoil, 0.0, -0.03 - recoil, 0.0, 0.01);

	// 오른쪽 가로 선
	crosshair.Draw(0.03 + recoil, 0.0, 0.1 + recoil, 0.0, 0.01);

	// 아래 세로 선
	crosshair.Draw(0.0, -0.1 - recoil, 0.0, -0.03 - recoil, 0.01);

	// 위 세로 선
	crosshair.Draw(0.0, 0.03 + recoil, 0.0, 0.1 + recoil, 0.01);
}