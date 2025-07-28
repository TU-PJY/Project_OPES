#include "ClearScreen.h"

void ClearScreen::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 2.0 * ASPECT, 2.0);
	SetColor(0.2, 0.2, 0.2);
	Render2D(TEX.ColorTex);

	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 3.5, 3.5);
	Render2D(TEX.UI_lobbyBackground, 0.3);

	text.Render(xmfloat2(0.0, 0.0), 0.2, "CLEAR");
}
