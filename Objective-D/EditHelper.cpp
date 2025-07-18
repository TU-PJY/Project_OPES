#include "EditHelper.h"
#include "CameraUtil.h"
#include "MathUtil.h"

EditHelper::EditHelper() {
	text.EnableShadow();
	text.SetShadow(XMFLOAT2(0.01, -0.01), 1.0);
}

void EditHelper::Update(float Delta) {
	XMFLOAT3 currentCamPosition = camera.GetPosition();
	float currentCamRotation = XMConvertToDegrees(camera.GetYaw());
	Math::Normalize2DAngleTo360(currentCamRotation);

	std::ostringstream ossX;
	std::ostringstream ossY;
	std::ostringstream ossZ;
	ossX << std::fixed << std::setprecision(3) << currentCamPosition.x;
	ossY << std::fixed << std::setprecision(3) << currentCamPosition.y;
	ossZ << std::fixed << std::setprecision(3) << currentCamPosition.z;
	
	renderStr =
		"(" + ossX.str() + ", " + ossY.str() + ", " + ossZ.str() + ") " +
		"(" + std::to_string((int)currentCamRotation) + " degrees" + ")";

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
}
