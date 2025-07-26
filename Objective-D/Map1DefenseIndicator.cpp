#include "Map1DefenseIndicator.h"
#include "TransformUtil.h"

DefenseIndicator::DefenseIndicator() {
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.01, -0.01), 0.5);
	totalRemain = GLOBAL.DefenseEnemyRemained;
	currentRemain = GLOBAL.DefenseEnemyRemained;
	prevRemain = GLOBAL.DefenseEnemyRemained;
}

void DefenseIndicator::Update(float Delta) {
	currentRemain = GLOBAL.DefenseEnemyRemained;
	renderString = std::to_string(totalRemain) + "/" + std::to_string(currentRemain);

	// 적이 처치될때마다 피드백을 보여준다.
	if (prevRemain != currentRemain) {
		sizeOffset = 0.1;
		prevRemain = currentRemain;
	}
	sizeOffset = std::lerp(sizeOffset, 0.0, 5.0 * Delta);

	// 디펜스모드 적을 모두 잡으면 화면 위로 올라가면서 삭제된다.
	if (currentRemain == 0) {
		renderHeight += Delta * 0.5;
		if (renderHeight >= 1.5)
			scene.DeleteObject(this);
	}
}

void DefenseIndicator::Render() {
	// 배경
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, 0.0, 1.0 - 0.15 + renderHeight);
	Transform::Scale2D(ScaleMatrix, 0.6 + sizeOffset, 0.3 + sizeOffset);
	Render2D(TEX.ColorTex, 0.5);

	// 적 이미지
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -0.2, 1.0 - 0.15 + renderHeight);
	Transform::Scale2D(ScaleMatrix, 0.2 + sizeOffset, 0.2 + sizeOffset);
	Render2D(TEX.UI_map1Enemy);

	// 텍스트
	text.Render(XMFLOAT2(0.07, 1.0 - 0.07 + renderHeight), 0.15 + sizeOffset, renderString);
}

