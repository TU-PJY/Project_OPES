#include "RandomUpgrade.h"
#include "RandomUtil.h"
#include "ModePack.h"

void SendPlayerUpgradePacket(int player_id, int random_id);

// 랜덤 버프/디버프 중에는 컨트롤 불가능
RandomUpgrade::RandomUpgrade() {
}

// 버프 부여
void RandomUpgrade::GetRandomBuff() {
	std::vector<int> availableItems{};
	int index{};
	// 현재 적용되지 않은 버프만 뽑는다.
	{
		std::lock_guard<std::mutex> lock(PacketMutex);
		for (int i = 1; i < 5; i++) {
			if (!GLOBAL.buff[i])
				availableItems.emplace_back(i);
		}

		index = Random.Gen(0, availableItems.size() - 1);
		GLOBAL.buff[availableItems[index]] = true;
		buffResult = availableItems[index];
	}

	SendPlayerUpgradePacket(GLOBAL.myID, availableItems[index]);
}

// 디버프 부여
void RandomUpgrade::GetRandomDebuff() {
	std::vector<int> availableItems{};
	int index{};
	// 현재 적용되지 않은 디버프만 뽑는다.
	{
		std::lock_guard<std::mutex> lock(PacketMutex);
		for (int i = 1; i < 5; i++) {
			if (!GLOBAL.deBuff[i])
				availableItems.emplace_back(i);
		}

		index = Random.Gen(0, availableItems.size() - 1);
		GLOBAL.deBuff[availableItems[index]] = true;
		debuffResult = availableItems[index];
	}

	SendPlayerUpgradePacket(GLOBAL.myID, -availableItems[index]);
}

void RandomUpgrade::Update(float delta) {
	// 마지막 스테이지의 경우 바로 삭제
	if (GLOBAL.stage == 3) {
		scene.SwitchMode(ClearMode::Start);
		return;
	}

	delay += delta;

	if (delay < 3.0) {
		// 아이콘을 위로 이동 및 회전시키면서 보여준다
		cardSize = std::lerp(cardSize, 0.7, 5.0 * delta);
		opacity = std::lerp(opacity, 1.0, 5.0 * delta);

		// 랜덤 버프 / 디버프 부여는 1회만 한다.
		if (!randomCreated) {
			{
				GetRandomBuff();
				GetRandomDebuff();
			}
			randomCreated = true;
		}
	}

	if (delay >= 3.0) {
		cardSize = std::lerp(cardSize, 1.2, 5.0 * delta);
		opacity = std::lerp(opacity, 0.0, 5.0 * delta);
		if (opacity <= 0.001) {
			if (GLOBAL.stage == 1)
				scene.SwitchMode(Level2::Start);

			else if (GLOBAL.stage == 2) 
				scene.SwitchMode(Level3::Start);
		}
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
	if (buffResult == MORE_GRENADE) {
		if (GLOBAL.myCharacter == CHARACTER_ENG)
			Render2D(TEX.UI_buff[0], opacity);
		else
			Render2D(TEX.UI_buff[buffResult], opacity);
	}
	else 
		Render2D(TEX.UI_buff[buffResult], opacity);

	// 디버프 아이콘 렌더링
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -0.7, 0.0);
	Transform::Scale2D(ScaleMatrix, cardSize, cardSize);
	Render2D(TEX.UI_deBuff[debuffResult], opacity);
}
