#include "BuffDebuffIndicator.h"

BuffDebuffIndicator::BuffDebuffIndicator() {
	for (int i = 1; i < 5; i++) {
		if (GLOBAL.buff[i])
			buffCount++;

		if (GLOBAL.deBuff[i])
			deBuffCount++;
	}

	float startBuffPos = -2.5;
	float startDebuffPos = 2.5 - 1.5;
	buffPosition.emplace_back(startBuffPos);
	deBuffPosition.emplace_back(startDebuffPos);

	for (int i = 0; i < buffCount; i++) {
		buffPosition.emplace_back(startBuffPos);
		startBuffPos += 1.5;
	}

	for (int i = 0; i < deBuffCount; i++) {
		deBuffPosition.emplace_back(startDebuffPos);
		startDebuffPos += 1.5;
	}
}

void BuffDebuffIndicator::Render() {
	if (buffCount == 0)
		return;

	// 버프 아이콘 배경
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, 0.0, -0.9);
	Transform::Scale2D(TranslateMatrix, 0.7, 0.2);
	SetColor(0.0, 0.0, 0.0);
	Render2D(TEX.ColorTex, 0.6);

	for (int i = 1; i < 5; i++) {
		if (GLOBAL.buff[i]) {
			BeginRender(RENDER_TYPE_2D);
			Transform::Move2D(TranslateMatrix, 0.0, -1.0);
			Transform::Scale2D(TranslateMatrix, 0.1, 0.1);
			Transform::Move2D(TranslateMatrix, 0.0, 1.0);
			Transform::Move2D(TranslateMatrix, buffPosition[i], 0.0);
			if (i == MORE_GRENADE) {
				if(GLOBAL.myCharacter == CHARACTER_ENG)
					Render2D(TEX.UI_buff[0]);
				else
					Render2D(TEX.UI_buff[i]);
			}
			else
				Render2D(TEX.UI_buff[i]);
		}

		if (GLOBAL.deBuff[i]) {
			BeginRender(RENDER_TYPE_2D);
			Transform::Move2D(TranslateMatrix, 0.0, -1.0);
			Transform::Scale2D(TranslateMatrix, 0.1, 0.1);
			Transform::Move2D(TranslateMatrix, 0.0, 1.0);
			Transform::Move2D(TranslateMatrix, deBuffPosition[i], 0.0);
			Render2D(TEX.UI_deBuff[i]);
		}
	}
}
