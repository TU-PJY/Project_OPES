#include "RandomUpgrade.h"
#include "RandomUtil.h"
#include "ModePack.h"

void SendPlayerUpgradePacket(int player_id, int random_id);

// 랜덤 버프/디버프 중에는 컨트롤 불가능
RandomUpgrade::RandomUpgrade() {
	SOUND.buff.Play();
	text.SetAlign(ALIGN_MIDDLE);
	text.SetHeightAlign(HEIGHT_MIDDLE);
	text.SetColor({ 1.0, 1.0, 1.0 });
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

	if (delay >= 5.0) {
		cardSize = std::lerp(cardSize, 1.2, 5.0 * delta);
		opacity = std::lerp(opacity, 0.0, 5.0 * delta);
		if (opacity <= 0.001) {
			// 확장 모드 활성화 시 확장 모드 실행
			if (GLOBAL.stage == 1) {
				if (GLOBAL.EXT_MODE_ENABLED)
					scene.SwitchMode(Level2EntryMode::Start);
				else
					scene.SwitchMode(Level2::Start);
			}

			else if (GLOBAL.stage == 2) {
				if (GLOBAL.EXT_MODE_ENABLED)
					scene.SwitchMode(Level3EntryMode::Start);
				else
					scene.SwitchMode(Level3::Start);
			}
		}
	}
}

void RandomUpgrade::Render() {
	// 배경 렌더링
	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, ASPECT * 2.0, 2.0);
	SetColor(0.0, 0.0, 0.0);
	Render2D(TEX.ColorTex, 0.7 * opacity);

	// 텍스트 렌더링
	text.SetOpacity(opacity);
	text.Render({ 0.0, 1.0 - 0.2 }, cardSize - 0.5, "Buff And Debuff!");

	// 버프 아이콘 렌더링
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, -0.7, 0.0);
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
	Transform::Move2D(TranslateMatrix, 0.7, 0.0);
	Transform::Scale2D(ScaleMatrix, cardSize, cardSize);
	Render2D(TEX.UI_deBuff[debuffResult], opacity);

	float textRenderHeight = -0.5;

	switch (buffResult) {
	case SHOOT_SPEED_INCREASE:
		text.Render({ -0.7, textRenderHeight }, cardSize - 0.6, "Fire rate +40%");
		break;

	case RELOAD_SPEED_INCREASE:
		text.Render({ -0.7, textRenderHeight }, cardSize - 0.6, "Reload speed +50%");
		break;

	case MORE_GRENADE: 
		if (GLOBAL.myCharacter == CHARACTER_ENG)
			text.Render({ -0.7, textRenderHeight }, cardSize - 0.6, "Install cool time -50%");
		else
			text.Render({ -0.7, textRenderHeight }, cardSize - 0.6, "Grenade +1");
		break;

	case DEFENSE_INCREASE:
		text.Render({ -0.7, textRenderHeight }, cardSize - 0.6, "Damage receive -20%");
		break;
	}

	switch (debuffResult) {
	case RECOIL_INCREASE:
		text.Render({ 0.7, textRenderHeight }, cardSize - 0.6, "Recoil +50%");
		break;

	case DAMAGE_REDUCE:
		text.Render({ 0.7, textRenderHeight }, cardSize - 0.6, "Gun damage -20%");
		break;

	case VISION_RANGE_REDUCE:
		text.Render({ 0.7, textRenderHeight }, cardSize - 0.6, "Vision range -50%");
		break;

	case WALK_ACC_REDUCE:
		text.Render({ 0.7, textRenderHeight }, cardSize - 0.6, "Walk acceleration -30%");
		break;
	}
}

//SHOOT_SPEED_INCREASE,
//RELOAD_SPEED_INCREASE,
//MORE_GRENADE, // (INSTALL_TIME_REDUCE)
//DEFENSE_INCREASE
//};
//
//enum BuffEng {
//	LESS_COOL_TIME = MORE_GRENADE
//};
//
//enum DebuffEnum {
//	DEBUFF_NONE,
//	RECOIL_INCREASE,
//	DAMAGE_REDUCE, // (수류탄, 터렛은 해당 X)
//	VISION_RANGE_REDUCE,
//	WALK_ACC_REDUCE
//};
