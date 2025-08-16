#include "AdventureTimer.h"

#include "Map1DefenseIndicator.h"
#include "TransformUtil.h"
#include "ClampUtil.h"

AdventureTimer::AdventureTimer() {
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.01, -0.01), 0.5);
}

void AdventureTimer::Update(float Delta) {
	renderHeight -= Delta * 0.5;
	Clamp::LimitValue(renderHeight, 0.0, CLAMP_DIR_LESS);
	renderString = std::to_string(currentTime);
}

void AdventureTimer::Render() {
	// 배경
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, 0.0, 1.0 - 0.15 + renderHeight);
	Transform::Scale2D(ScaleMatrix, 0.6, 0.3 + sizeOffset);
	Render2D(TEX.ColorTex, 0.5);

	// 텍스트
	text.Render(XMFLOAT2(0.0, 1.0 - 0.07 + renderHeight), 0.15, renderString);
}

void AdventureTimer::InputTime(int time) {
	currentTime = time;
}