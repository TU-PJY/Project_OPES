#include "GameObject.h"
#include "CameraUtil.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"

// GameObject Ŭ������ ��� ��ü���� ��ӹ޴� �θ� Ŭ�����̴�.
// ��� ��ü�� �ݵ�� �� Ŭ�����κ��� ��ӹ޾ƾ� Scene�� ��ü�� ������Ʈ�ϰ� �������Ѵ�.

// ��ü�� ������ ���¸� �ʱ�ȭ �Ѵ�. ��� ��ü�� �� �Լ��� ������ ������ ���۵ȴ�.
// �⺻ RANDER_TYPE_3D�� Ÿ���� �����Ǿ��ִ�.
// RENDER_TYPE_3D_STATIC�Ǵ� RENDER_TYPE_2D_STATIC�� ��� ��� �ʱ�ȭ�� �������� �ʴ´�. 
void GameObject::BeginRender(int RenderTypeFlag) {
	// ��� ��� ����
	RenderType = RenderTypeFlag;

	if (RenderTypeFlag != RENDER_TYPE_3D_STATIC && RenderTypeFlag != RENDER_TYPE_2D_STATIC) {
		Transform::Identity(TranslateMatrix);
		Transform::Identity(RotateMatrix);
		Transform::Identity(ScaleMatrix);
	}

	if (RenderTypeFlag == RENDER_TYPE_2D || RenderTypeFlag == RENDER_TYPE_2D_STATIC) {
		// 2d ������Ʈ ��¿� ��Ʈ �ñ״�ó�� ����
		GlobalCommandList->SetGraphicsRootSignature(ImageShaderRootSignature);

		Transform::Identity(ImageAspectMatrix);
		FlipTexture(FLIP_TYPE_NONE);
	}

	if (RenderTypeFlag == RENDER_TYPE_3D || RenderTypeFlag == RENDER_TYPE_3D_STATIC || RenderTypeFlag == RENDER_TYPE_3D_ORTHO) {
		// 3d ������Ʈ ��¿� ��Ʈ �ñ״�ó�� ����
		GlobalCommandList->SetGraphicsRootSignature(ObjectShaderRootSignature);
		
		// �ɼǿ� ���� �Ȱ��� �켱 ��Ȱ��ȭ �ǰų� �켱 Ȱ��ȭ �ȴ�.
		if (ENABLE_FOG_AFTER_BEGIN_RENDER)
			SetFogUse(ENABLE_FOG);
		else
			SetFogUse(DISABLE_FOG);

		// �ɼǿ� ���� �ؽ�ó ���� ������ ����ǰų� ������� �ʴ´�.
		if (ENABLE_TEXTURE_V_FLIP_AFTER_BEGIN_RENDER)
			FlipTexture(FLIP_TYPE_V);
		else
			FlipTexture(FLIP_TYPE_NONE);
	}

	// �Ž� ���� �ʱ�ȭ
	SetColor(XMFLOAT3(0.0, 0.0, 0.0));

	// �ؽ�ó ���� �ʱ�ȭ
	ObjectAlpha = 1.0f;

	// ��� ��� ����
	switch(RenderTypeFlag) {
	case RENDER_TYPE_2D: case RENDER_TYPE_2D_STATIC:
		camera.SetToStaticMode();
		break;

	case RENDER_TYPE_3D: case RENDER_TYPE_3D_STATIC:
		SetLightUse(ENABLE_LIGHT);
		camera.SetToDefaultMode();
		break;
	}
}

// ��ü �޽��� ������ �����Ѵ�.
void GameObject::SetColor(XMFLOAT3& Color) {
	ObjectColor = Color;
}

// ��ü �޽��� ������ �����Ѵ�.
void GameObject::SetColor(float R, float G, float B) {
	ObjectColor.x = R;
	ObjectColor.y = G;
	ObjectColor.z = B;
}

// RGB���� ����Ͽ� ��ü �Ž��� ������ �����Ѵ�.
void GameObject::SetColorRGB(float R, float G, float B) {
	ObjectColor.x = 1.0 / 255.0 * float(R);
	ObjectColor.y = 1.0 / 255.0 * float(G);
	ObjectColor.z = 1.0 / 255.0 * float(B);
}

// �ؽ�ó�� ������Ų��. �𵨿� ���� �ٸ��� ����� �� �ִ�.
void GameObject::FlipTexture(int FlipType) {
	// �̹��� ��� ��忡���� ���� ������ �⺻ �����̱� ������ ������ �ٸ��� �����Ѵ�.
	if (RenderType == RENDER_TYPE_2D || RenderType == RENDER_TYPE_2D_STATIC) {
		switch (FlipType) {
		case FLIP_TYPE_V:
			CBVUtil::Input(GlobalCommandList, FlipCBV, FLIP_TYPE_NONE); break;

		case FLIP_TYPE_H:
			CBVUtil::Input(GlobalCommandList, FlipCBV, FLIP_TYPE_HV);   break;

		case FLIP_TYPE_HV:
			CBVUtil::Input(GlobalCommandList, FlipCBV, FLIP_TYPE_H);    break;

		case FLIP_TYPE_NONE:
			CBVUtil::Input(GlobalCommandList, FlipCBV, FLIP_TYPE_V);    break;
		}
	}

	else
		CBVUtil::Input(GlobalCommandList, FlipCBV, FlipType);
}

// ���� Ȱ��ȭ / ��Ȱ��ȭ
void GameObject::SetLightUse(int Flag) {
	CBVUtil::Input(GlobalCommandList, BoolLightCBV, Flag);
}

// �Ȱ� Ȱ��ȭ / ��Ȱ��ȭ
void GameObject::SetFogUse(int Flag) {
	CBVUtil::Input(GlobalCommandList, BoolFogCBV, Flag);
}

// 3D ������
void GameObject::Render3D(Mesh* MeshPtr, Texture* TexturePtr, float AlphaValue, int DepthTestFlag) {
	TexturePtr->Render3D(GlobalCommandList);

	switch (DepthTestFlag) {
	case DEPTH_TEST_DEFAULT:
		ObjectShader->RenderDefault(GlobalCommandList);  
		break;

	case DEPTH_TEST_NONE:
		ObjectShader->RenderDepthNone(GlobalCommandList); 
		break;

	case DEPTH_TEST_FPS:
		ObjectShader->RenderFPS(GlobalCommandList);
		break;
	}

	ObjectAlpha = AlphaValue;
	CBVUtil::Input(GlobalCommandList, LightCBV);
	CBVUtil::Input(GlobalCommandList, FogCBV);

	PrepareRender();
	MeshPtr->Render(GlobalCommandList);
}

// 2D ������
void GameObject::Render2D(Texture* TexturePtr, float AlphaValue, bool EnableAspect) {
	if(EnableAspect)
		Transform::ImageAspect(ImageAspectMatrix, TexturePtr->Width, TexturePtr->Height);
	TexturePtr->Render2D(GlobalCommandList);
	ImageShader->RenderDepthNone(GlobalCommandList);
	ObjectAlpha = AlphaValue;

	PrepareRender();
	SysRes.ImagePannel->Render(GlobalCommandList);
}

// ���콺 ������κ��� ȸ���� ������Ʈ �Ѵ�.
void GameObject::UpdateMotionRotation(float& RotationX, float& RotationY, float DeltaX, float DeltaY) {
	RotationX += DeltaY;
	RotationY += DeltaX;
}

// ���콺 ������κ��� ȸ���� ������Ʈ �Ѵ�.
void GameObject::UpdateMotionRotation(XMFLOAT3& Rotation, float DeltaX, float DeltaY) {
	Rotation.x += DeltaY;
	Rotation.y += DeltaX;
}

// ��ŷ�� ���� ����� ������Ʈ �Ѵ�. ������ ���� ����ϵ��� �Ѵ�.
void GameObject::UpdatePickMatrix() {
	PickMatrix = ResultMatrix;
}

// ���� ��Ⱦ�� ���Ѵ�. UI�� �����Ҷ� �ַ� ����Ѵ�.
float GameObject::ASP(float Value) {
	return ASPECT * Value;
}

// FBX �ִϸ��̼��� ������Ʈ �Ѵ�. ������ �ð��� �־��ָ� �ڵ����� ����ȴ�.
// HEAP_TYPE_DEFAULT�� ������ FBX�� �ִϸ��̼� ������Ʈ�� �� �� ����.
void GameObject::UpdateFBXAnimation(FBXMesh& TargetMesh, float FrameTime) {
	TargetMesh.CurrentTime += FrameTime;
	if (TargetMesh.CurrentTime >= TargetMesh.TotalTime) {
		float OverTime = TargetMesh.CurrentTime - TargetMesh.TotalTime;
		TargetMesh.CurrentTime = OverTime;
	}
	for (auto const& M : TargetMesh.MeshPart)
		M->UpdateSkinning(TargetMesh.CurrentTime);
}

// ���� �ִϸ��̼� �� �ϳ��� �����Ѵ�.
void GameObject::SelectFBXAnimation(FBXMesh& TargetMesh, std::string AnimationName) {
	fbxUtil.SelectAnimation(TargetMesh, AnimationName);
}

// ���� �ִϸ��̼� ��� �ð��� �ʱ�ȭ �Ѵ�.
void GameObject::ResetAnimationTime(FBXMesh& TargetMesh) {
	TargetMesh.CurrentTime = 0.0;
}

// FBX ������ �Լ��̴�. ������ FBX �� ������ �� �� �Լ��� ������ �Ѵ�.
// Render3D�� ������ �ڵ带 �ִ��� �ǵ��� �ʱ� ���� ���ܵд�.
void GameObject::RenderFBX(FBXMesh& TargetMesh, Texture* TexturePtr, float AlphaValue, float DepthTestFlag){
	for (auto const& M : TargetMesh.MeshPart)
		Render3D(M, TexturePtr, AlphaValue, DepthTestFlag);
}

// FBX �Ž����� ��ŷ �˻� �Լ��̴�. ��κ��� ��Ű�� �ִϸ��̼��� ����ϴ� FBX ���ϵ��� 1~2���� �Ž��� ������ ���� ���̹Ƿ� ���� �� ū �������� �������̴�. (���ٸ� �ְ�����...)
int GameObject::PickRayFBX(FBXMesh& TargetMesh, XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, float* HitDistance) {
	int TotelInterSected{};
	for (auto const& M : TargetMesh.MeshPart) {
		int InterSected{};
		XMVECTOR PickRayOrigin, PickRayDirecton;
		GenPickingRay(PickPosition, ViewMatrix, PickRayOrigin, PickRayDirecton);
		InterSected = M->CheckRayIntersection(PickRayOrigin, PickRayDirecton, HitDistance);
		TotelInterSected += InterSected;
	}

	return TotelInterSected;
}

// ��ŷ �� ����ϴ� �Լ��̴�. ���α׷��Ӱ� �� �Լ��� ���� ����� ���� ����.
int GameObject::PickRayInter(Mesh* MeshPtr, XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, float* HitDistance) {
	int InterSected{};

	XMVECTOR PickRayOrigin, PickRayDirecton;
	GenPickingRay(PickPosition, ViewMatrix, PickRayOrigin, PickRayDirecton);
	InterSected = MeshPtr->CheckRayIntersection(PickRayOrigin, PickRayDirecton, HitDistance);

	return(InterSected);
}

// ĳ���� ��Ʈ�ѷ��� ����ϴ� �Լ��̴�.
// �� �̵� bool ������ �ϳ��� Ű�� �����Ѵ�.
void GameObject::InputBoolSwitch(int SwitchFlag, KeyEvent& Event, WPARAM Key, bool& BoolValue) {
	if (Key == Event.Key) {
		if (SwitchFlag == KEY_DOWN_TRUE) {
			if (Event.Type == WM_KEYDOWN)
				BoolValue = true;
			else if (Event.Type == WM_KEYUP)
				BoolValue = false;
		}

		else if (SwitchFlag == KEY_UP_TRUE) {
			if (Event.Type == WM_KEYUP)
				BoolValue = true;
			else if (Event.Type == WM_KEYDOWN)
				BoolValue = false;
		}
	}
}

//////////////////////////////////////// private

// ��İ� ���̴� �� ���� ���� ������ ���̴��� �����Ѵ�. Render�Լ��� �����ϸ� �� �Լ��� ����ȴ�. ��, ���� ����� ���� ����.
void GameObject::PrepareRender() {
	SetCamera();

	ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&RotateMatrix), XMLoadFloat4x4(&TranslateMatrix));
	ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), ResultMatrix);
	//ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&AnimationMatrix), ResultMatrix);

	// �̹��� ��� ����ϰ�� ��Ⱦ�� �����Ѵ�.
	if (RenderType == RENDER_TYPE_2D || RenderType == RENDER_TYPE_2D_STATIC)
		ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ImageAspectMatrix), ResultMatrix);

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(GlobalCommandList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &ObjectColor, GAME_OBJECT_INDEX, 3, 16);
	RCUtil::Input(GlobalCommandList, &ObjectAlpha, GAME_OBJECT_INDEX, 1, 19);
}

// ������ �� ī�޶� �����Ѵ�.
void GameObject::SetCamera() {
	// ī�޶� �� ����� �����Ѵ�.
	camera.SetViewMatrix();

	// ���� Ÿ�Կ� ���� �ٸ� ����� �ʱ�ȭ �Ѵ�.
	switch (RenderType) {
	case RENDER_TYPE_3D: case RENDER_TYPE_3D_STATIC:
		camera.GeneratePerspectiveMatrix(0.1f, 500.0f, ASPECT, 45.0f + global_fov_offset);
		break;

	case RENDER_TYPE_3D_ORTHO:
		camera.GenerateOrthoMatrix(1.0, 1.0, ASPECT, 0.0f, 10.0f);
		break;

	case RENDER_TYPE_2D: case RENDER_TYPE_2D_STATIC:
		camera.GenerateStaticMatrix();
	}

	camera.SetViewportsAndScissorRects();
	camera.UpdateShaderVariables();
}

// ��ŷ �� ����ϴ� �Լ��̴�. ���α׷��Ӱ� �� �Լ��� ���� ����� ���� ����.
void GameObject::GenPickingRay(XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, XMVECTOR& PickRayOrigin, XMVECTOR& PickRayDirection) {
	XMMATRIX MatrixTomodel = XMMatrixInverse(NULL, PickMatrix * ViewMatrix);
	XMFLOAT3 CameraOrigin(0.0f, 0.0f, 0.0f);
	PickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&CameraOrigin), MatrixTomodel);
	PickRayDirection = XMVector3TransformCoord(PickPosition, MatrixTomodel);
	PickRayDirection = XMVector3Normalize(PickRayDirection - PickRayOrigin);
}