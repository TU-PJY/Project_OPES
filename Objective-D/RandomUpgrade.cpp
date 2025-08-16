#include "RandomUpgrade.h"
#include "RandomUtil.h"

void SendPlayerUpgradePacket(int player_id, int random_id);

// 랜덤 버프/디버프 중에는 컨트롤 불가능
RandomUpgrade::RandomUpgrade() {
	GLOBAL.controlEnabled = false;
}

// 버프 부여
void RandomUpgrade::GetRandomBuff() {
	int randNum = Random.Gen(0, 3);
	GLOBAL.buff[randNum] = true;
	SendPlayerUpgradePacket(GLOBAL.myID, randNum);
}

// 디버프 부여
void RandomUpgrade::GetRandomDebuff() {
	int randNum = Random.Gen(0, 3);
	GLOBAL.deBuff[randNum] = true;
	SendPlayerUpgradePacket(GLOBAL.myID, -randNum);
}

void RandomUpgrade::Update(float delta) {
	delay += delta;

	if (delay >= 1.0 && delay < 5.0) {
		// 아이콘을 위로 이동 및 회전시키면서 보여준다
		cardSize = std::lerp(cardSize, 0.7, 5.0 * delta);
		opacity = std::lerp(opacity, 1.0, 5.0 * delta);

		// 랜덤 버프 / 디버프 부여는 1회만 한다.
		if (!randomCreated) {
			GetRandomBuff();
			GetRandomDebuff();
			randomCreated = true;
		}
	}

	if (delay >= 5.0) {
		cardSize = std::lerp(cardSize, 1.2, 5.0 * delta);
		opacity = std::lerp(opacity, 0.0, 5.0 * delta);
		if (opacity <= 0.001)
			scene.DeleteObject(this);
	}
}

void RandomUpgrade::Render() {
	// 배경 렌더링
	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, ASPECT * 2.0, 2.0);
	SetColor(0.0, 0.0, 0.0);
	Render2D(TEX.ColorTex, 0.7 * opacity);

	// 버프 아이콘 렌더링
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, 0.7, 0.0);
	Transform::Scale2D(ScaleMatrix, cardSize, cardSize);
	SetColor(0.0, 1.0, 0.0);
	Render2D(TEX.ColorTex, opacity);

	// 디버프 아이콘 렌더링
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -0.7, 0.0);
	Transform::Scale2D(ScaleMatrix, cardSize, cardSize);
	SetColor(1.0, 0.0, 0.0);
	Render2D(TEX.ColorTex, opacity);
}
