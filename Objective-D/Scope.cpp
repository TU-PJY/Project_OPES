#include "Scope.h"
#include "ClampUtil.h"

void Scope::Render() {
	if (!renderState)
		return;

	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, size, size);
	Render2D(TEX.scope);
}

void Scope::Update(float Delta) {
	size = std::lerp(size, 5.0, 10.0 * Delta);
}

void Scope::SetRenderState(bool flag) {
	renderState = flag;
}

void Scope::SetSize(float Size) {
	size = Size;
}
