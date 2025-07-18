#include "EditHelper.h"
#include "CameraUtil.h"
#include "MathUtil.h"

EditHelper::EditHelper(const std::string& currentMapName) {
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.008, -0.008), 1.0);
	this->currentMapName = currentMapName;
}

void EditHelper::Update(float Delta) {
	XMFLOAT3 pickPosition{};
	if (auto terrain = scene.Find(currentMapName); terrain)
		pickPosition = terrainUtil.CheckCollisionRay(terrain->GetTerrain());

	XMFLOAT3 currentCamPosition = camera.GetPosition();
	float currentCamRotation = XMConvertToDegrees(camera.GetYaw());
	Math::Normalize2DAngleTo360(currentCamRotation);

	std::ostringstream ossX;
	std::ostringstream ossY;
	std::ostringstream ossZ;
	ossX << std::fixed << std::setprecision(3) << currentCamPosition.x;
	ossY << std::fixed << std::setprecision(3) << currentCamPosition.y;
	ossZ << std::fixed << std::setprecision(3) << currentCamPosition.z;

	std::ostringstream ossPickX;
	std::ostringstream ossPickY;
	std::ostringstream ossPickZ;
	ossPickX << std::fixed << std::setprecision(3) << pickPosition.x;
	ossPickY << std::fixed << std::setprecision(3) << pickPosition.y;
	ossPickZ << std::fixed << std::setprecision(3) << pickPosition.z;
	
	renderStr =
		"(" + ossX.str() + ", " + ossY.str() + ", " + ossZ.str() + ") " +
		"(" + std::to_string((int)currentCamRotation) + " degrees" + ") " +
		"(" + ossPickX.str() + ", " + ossPickY.str() + ", " + ossPickZ.str() + ")";

	//Z+
	compassPos[0].x = compassCenter.x + cosf(XMConvertToRadians(currentCamRotation - 270.0)) * 0.25;
	compassPos[0].y = compassCenter.y + sinf(XMConvertToRadians(currentCamRotation - 270.0)) * 0.25;

	//Z-
	compassPos[1].x = compassCenter.x + cosf(XMConvertToRadians(currentCamRotation - 90.0)) * 0.25;
	compassPos[1].y = compassCenter.y + sinf(XMConvertToRadians(currentCamRotation - 90.0)) * 0.25;

	// X+
	compassPos[2].x = compassCenter.x + cosf(XMConvertToRadians(currentCamRotation)) * 0.25;
	compassPos[2].y = compassCenter.y + sinf(XMConvertToRadians(currentCamRotation)) * 0.25;

	// X-
	compassPos[3].x = compassCenter.x + cosf(XMConvertToRadians(currentCamRotation - 180.0)) * 0.25;
	compassPos[3].y = compassCenter.y + sinf(XMConvertToRadians(currentCamRotation - 180.0)) * 0.25;
}

void EditHelper::Render() {
	text.Render(XMFLOAT2(0.0, -0.8), 0.1, renderStr);
	text.Render(compassPos[0], 0.1, "Z+");
	text.Render(compassPos[1], 0.1, "Z-");
	text.Render(compassPos[2], 0.1, "X+");
	text.Render(compassPos[3], 0.1, "X-");

	BeginRender(RENDER_TYPE_2D);
	Transform::Scale2D(ScaleMatrix, 0.02, 0.02);
	SetColor(XMFLOAT3(1.0, 0.0, 0.0));
	Render2D(TEX.ColorTex);
}
