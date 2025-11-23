#include "RoadBlock.h"
#include "MathUtil.h"
#include "CameraUtil.h"

RoadBlock::RoadBlock(const XMFLOAT3& createPosition, float degrees, int num) {
	position = createPosition;
	rotation.y = degrees;
	numBlock = num;
	oobb.Update(position, XMFLOAT3(size.x * (float)numBlock * 0.5, size.y * 0.5, 0.5), XMFLOAT3(0.0, rotation.y, 0.0));
}

void RoadBlock::Update(float Delta) {
	if (GLOBAL.mapName == "map1" && !GLOBAL.Map1DefenseState) {
		position.y -= 5.0 * Delta;
		if (position.y <= -4.0) {
			GLOBAL.mapOOBBdata.pop_back();
			scene.DeleteObject(this);
		}
	}

	if (GLOBAL.mapName == "map2" && !GLOBAL.Map2DefenseState) {
		position.y -= 5.0 * Delta;
		if (position.y <= -4.0) {
			GLOBAL.mapOOBBdata.pop_back();
			scene.DeleteObject(this);
		}
	}

	if (GLOBAL.mapName=="map3" && !GLOBAL.Map3DefenseState) {
		position.y -= 5.0 * Delta;
		if (position.y <= -4.0) {
			GLOBAL.mapOOBBdata.pop_back();
			scene.DeleteObject(this);
		}
	}
}

void RoadBlock::Render() {
	// ¿ìÃø, ÁÂÃøÀ¸·Î ³ª´©¾î ·»´õ¸µ
	// ¿ìÃø ·»´õ¸µ
	// Áß¾ÓºÎÅÍ ·»´õ¸µ
	float radians = XMConvertToRadians(rotation.y);
	XMFLOAT3 startPosition = XMFLOAT3(position.x - cos(radians) * size.x * numBlock / 2, position.y, position.z + sin(radians) * size.x * numBlock / 2);

	for (int i = 0; i < numBlock; i++) {
		BeginRender();
		SetLightUse(DISABLE_LIGHT);
		XMFLOAT3 offset = XMFLOAT3(size.x * i, 0.0, 0.0);
		Transform::Move(TranslateMatrix, startPosition);
		Transform::Rotate(TranslateMatrix, rotation);
		Transform::Move(TranslateMatrix, offset);
		Transform::Scale(ScaleMatrix, size);

		Render3D(SYSRES.BillboardMesh, TEX.roadBlock);
	}

	oobb.Render();
}

OOBB RoadBlock::GetOOBB() {
	return oobb;
}
