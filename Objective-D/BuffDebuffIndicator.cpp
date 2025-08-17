#include "BuffDebuffIndicator.h"

BuffDebuffIndicator::BuffDebuffIndicator() {
	renderCount = GLOBAL.stage - 1;

	std::lock_guard<std::mutex> lock(PacketMutex);
	for (int i = 0; i < 5; i++) {
		if (GLOBAL.buff[i])
			buffState[i] = true;

		if (GLOBAL.deBuff[i])
			debuffState[i] = true;
	}
}

void BuffDebuffIndicator::Render() {
	if (renderCount == 0)
		return;

	// 버프 아이콘 배경
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, 0.0, -0.9);
	Transform::Scale2D(TranslateMatrix, 0.7, 0.2);
	SetColor(0.0, 0.0, 0.0);
	Render2D(TEX.ColorTex, 0.6);

	float renderBuffPos = -2.5;
	float renderDebuffPos = 2.5 - 1.5;

	for (int i = 1; i < 5; i++) {
		if (buffState[i]) {
			BeginRender(RENDER_TYPE_2D);
			Transform::Move2D(TranslateMatrix, 0.0, -1.0);
			Transform::Scale2D(TranslateMatrix, 0.1, 0.1);
			Transform::Move2D(TranslateMatrix, 0.0, 1.0);
			Transform::Move2D(TranslateMatrix, renderBuffPos, 0.0);
			if (i == MORE_GRENADE) {
				if(GLOBAL.myCharacter == CHARACTER_ENG)
					Render2D(TEX.UI_buff[0]);
				else
					Render2D(TEX.UI_buff[i]);
			}
			else
				Render2D(TEX.UI_buff[i]);
		}

		if (debuffState[i]) {
			BeginRender(RENDER_TYPE_2D);
			Transform::Move2D(TranslateMatrix, 0.0, -1.0);
			Transform::Scale2D(TranslateMatrix, 0.1, 0.1);
			Transform::Move2D(TranslateMatrix, 0.0, 1.0);
			Transform::Move2D(TranslateMatrix, renderDebuffPos, 0.0);
			Render2D(TEX.UI_deBuff[i]);
		}

		renderBuffPos += 1.5;
		renderDebuffPos += 1.5;
	}
}
