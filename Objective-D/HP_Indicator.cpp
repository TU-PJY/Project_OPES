#include "HP_Indicator.h"
#include "CameraUtil.h"

HP_Indicator::HP_Indicator() {
	Math::InitVector(vec);
}

// 체력 바에 위치를 전달한다. 
void HP_Indicator::InputPosition(XMFLOAT3& inputPos, float heightOffset) {
	position = XMFLOAT3(inputPos.x, inputPos.y + heightOffset, inputPos.z);
}

// 자신의 전체 체력과 현재 남은 체력을 전달한다.
void HP_Indicator::InputHP(int fullHP, int currentHP){
	full_hp = fullHP;
	current_hp = currentHP;
	length = (float)current_hp / (float)full_hp * 2.95;
}

// hp바는 항상 카메라를 향하도록 한다
void HP_Indicator::Update(float Delta) {

}

void HP_Indicator::Render() {
	BeginRender();
	Transform::Move(TranslateMatrix, position);
	Math::BillboardLookAt(RotateMatrix, vec, position, camera.GetPosition());
	Transform::Scale(ScaleMatrix, 3.0, 0.2, 1.0);
	SetColor(0.0, 0.0, 0.0);
	Render3D(SysRes.BillboardMesh, TEX.ColorTex);

	BeginRender();
	Transform::Move(TranslateMatrix, position.x, position.y, position.z);
	Math::BillboardLookAt(RotateMatrix, vec, position, camera.GetPosition());
	Transform::Move(RotateMatrix, 0.0, 0.0, -0.01);
	Transform::Scale(ScaleMatrix, length, 0.15, 1.0);
	SetColor(1.0, 0.0, 0.0);
	Render3D(SysRes.BillboardMesh, TEX.ColorTex);
}
