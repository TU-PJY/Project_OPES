#include "SpecterUI.h"

SpecterUI::SpecterUI() {
	text.EnableShadow();
	text.SetShadow(xmfloat2(0.01, -0.01), 0.7);
}

void SpecterUI::Render() {
	BeginRender(RENDER_TYPE_2D);
	Transform::Move2D(TranslateMatrix, 0.0, -1.0);
	Transform::Scale2D(ScaleMatrix, 2.0 * ASPECT, 0.5);
	Render2D(TEX.ColorTex, 0.5);

	text.Render(xmfloat2(0.0, -1.0), 0.2, "Watching players...");
}
