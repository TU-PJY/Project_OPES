#include "PlayerHit.h"

// 점차 투명해지다가 완전히 투명해지면 삭제된다
void PlayerHit::Update(float Delta) {
	opacity -= Delta * 2.0;
	if (opacity <= 0.0)
		scene.DeleteObject(this);
}

void PlayerHit::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 2.0 * ASPECT, 2.0);
	Render2D(TEX.UI_playerHit, opacity);
}
