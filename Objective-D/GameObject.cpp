#include "GameObject.h"
#include "CameraUtil.h"
#include "CBVUtil.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"

// GameObject 클래스는 모든 객체들이 상속받는 부모 클래스이다.
// 모든 객체는 반드시 이 클래스로부터 상속받아야 Scene이 객체를 업데이트하고 렌더링한다.

// 객체의 렌더링 상태를 초기화 한다. 모든 객체는 이 함수로 렌더링 과정이 시작된다.
// 기본 RANDER_TYPE_3D로 타입이 지정되어있다.
// RENDER_TYPE_3D_STATIC또는 RENDER_TYPE_2D_STATIC일 경우 행렬 초기화를 실행하지 않는다. 
void GameObject::BeginRender(int RenderTypeFlag) {
	// 출력 모드 지정
	RenderType = RenderTypeFlag;

	if (RenderTypeFlag != RENDER_TYPE_3D_STATIC && RenderTypeFlag != RENDER_TYPE_2D_STATIC) {
		Transform::Identity(TranslateMatrix);
		Transform::Identity(RotateMatrix);
		Transform::Identity(ScaleMatrix);
	}

	if (RenderTypeFlag == RENDER_TYPE_2D || RenderTypeFlag == RENDER_TYPE_2D_STATIC) {
		// 2d 오브젝트 출력용 루트 시그니처로 변경
		GlobalCommandList->SetGraphicsRootSignature(ImageShaderRootSignature);

		Transform::Identity(ImageAspectMatrix);
		FlipTexture(FLIP_TYPE_NONE);
	}

	if (RenderTypeFlag == RENDER_TYPE_3D || RenderTypeFlag == RENDER_TYPE_3D_STATIC || RenderTypeFlag == RENDER_TYPE_3D_ORTHO) {
		// 3d 오브젝트 출력용 루트 시그니처로 변경
		GlobalCommandList->SetGraphicsRootSignature(ObjectShaderRootSignature);
		
		// 옵션에 따라 안개가 우선 비활성화 되거나 우선 활성화 된다.
		if (ENABLE_FOG_AFTER_BEGIN_RENDER)
			SetFogUse(ENABLE_FOG);
		else
			SetFogUse(DISABLE_FOG);

		// 옵션에 따라 텍스처 수직 반전이 적용되거나 적용되지 않는다.
		if (ENABLE_TEXTURE_V_FLIP_AFTER_BEGIN_RENDER)
			FlipTexture(FLIP_TYPE_V);
		else
			FlipTexture(FLIP_TYPE_NONE);
	}

	// 매쉬 색상 초기화
	SetColor(XMFLOAT3(0.0, 0.0, 0.0));

	// 텍스처 상태 초기화
	ObjectAlpha = 1.0f;

	// 출력 모드 변경
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

// 객체 메쉬의 색상을 설정한다.
void GameObject::SetColor(XMFLOAT3& Color) {
	ObjectColor = Color;
}

// 객체 메쉬의 색상을 설정한다.
void GameObject::SetColor(float R, float G, float B) {
	ObjectColor.x = R;
	ObjectColor.y = G;
	ObjectColor.z = B;
}

// RGB값을 사용하여 객체 매쉬의 색상을 설정한다.
void GameObject::SetColorRGB(float R, float G, float B) {
	ObjectColor.x = 1.0 / 255.0 * float(R);
	ObjectColor.y = 1.0 / 255.0 * float(G);
	ObjectColor.z = 1.0 / 255.0 * float(B);
}

// 텍스처를 반전시킨다. 모델에 따라 다르게 사용할 수 있다.
void GameObject::FlipTexture(int FlipType) {
	// 이미지 출력 모드에서는 수직 반전이 기본 적용이기 때문에 반전이 다르게 동작한다.
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

// 조명 활성화 / 비활성화
void GameObject::SetLightUse(int Flag) {
	CBVUtil::Input(GlobalCommandList, BoolLightCBV, Flag);
}

// 안개 활성화 / 비활성화
void GameObject::SetFogUse(int Flag) {
	CBVUtil::Input(GlobalCommandList, BoolFogCBV, Flag);
}

// 3D 렌더링
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

// 2D 렌더링
void GameObject::Render2D(Texture* TexturePtr, float AlphaValue, bool EnableAspect) {
	if(EnableAspect)
		Transform::ImageAspect(ImageAspectMatrix, TexturePtr->Width, TexturePtr->Height);
	TexturePtr->Render2D(GlobalCommandList);
	ImageShader->RenderDepthNone(GlobalCommandList);
	ObjectAlpha = AlphaValue;

	PrepareRender();
	SysRes.ImagePannel->Render(GlobalCommandList);
}

// 마우스 모션으로부터 회전값 업데이트 한다.
void GameObject::UpdateMotionRotation(float& RotationX, float& RotationY, float DeltaX, float DeltaY) {
	RotationX += DeltaY;
	RotationY += DeltaX;
}

// 마우스 모션으로부터 회전값 업데이트 한다.
void GameObject::UpdateMotionRotation(XMFLOAT3& Rotation, float DeltaX, float DeltaY) {
	Rotation.x += DeltaY;
	Rotation.y += DeltaX;
}

// 피킹을 위한 행렬을 업데이트 한다. 렌더링 직후 사용하도록 한다.
void GameObject::UpdatePickMatrix() {
	PickMatrix = ResultMatrix;
}

// 값에 종횡비를 곱한다. UI를 구현할때 주로 사용한다.
float GameObject::ASP(float Value) {
	return ASPECT * Value;
}

// FBX 애니메이션을 업데이트 한다. 프레임 시간을 넣어주면 자동으로 재생된다.
// HEAP_TYPE_DEFAULT로 지정된 FBX는 애니메이션 업데이트를 할 수 없다.
void GameObject::UpdateFBXAnimation(FBXMesh& TargetMesh, float FrameTime) {
	TargetMesh.CurrentTime += FrameTime;
	if (TargetMesh.CurrentTime >= TargetMesh.TotalTime) {
		float OverTime = TargetMesh.CurrentTime - TargetMesh.TotalTime;
		TargetMesh.CurrentTime = OverTime;
	}
	for (auto const& M : TargetMesh.MeshPart)
		M->UpdateSkinning(TargetMesh.CurrentTime);
}

// 여러 애니메이션 중 하나를 선택한다.
void GameObject::SelectFBXAnimation(FBXMesh& TargetMesh, std::string AnimationName) {
	fbxUtil.SelectAnimation(TargetMesh, AnimationName);
}

// 현재 애니메이션 재생 시간을 초기화 한다.
void GameObject::ResetAnimationTime(FBXMesh& TargetMesh) {
	TargetMesh.CurrentTime = 0.0;
}

// FBX 렌더용 함수이다. 앞으로 FBX 모델 렌더링 시 이 함수를 쓰도록 한다.
// Render3D는 기존의 코드를 최대한 건들지 않기 위해 남겨둔다.
void GameObject::RenderFBX(FBXMesh& TargetMesh, Texture* TexturePtr, float AlphaValue, float DepthTestFlag){
	for (auto const& M : TargetMesh.MeshPart)
		Render3D(M, TexturePtr, AlphaValue, DepthTestFlag);
}

// FBX 매쉬전용 피킹 검사 함수이다. 대부분의 스키닝 애니메이션을 사용하는 FBX 파일들은 1~2개의 매쉬를 가지고 있을 것이므로 성능 상 큰 오버헤드는 없을것이다. (많다면 있겠지만...)
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

// 피킹 시 사용하는 함수이다. 프로그래머가 이 함수를 직접 사용할 일은 없다.
int GameObject::PickRayInter(Mesh* MeshPtr, XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, float* HitDistance) {
	int InterSected{};

	XMVECTOR PickRayOrigin, PickRayDirecton;
	GenPickingRay(PickPosition, ViewMatrix, PickRayOrigin, PickRayDirecton);
	InterSected = MeshPtr->CheckRayIntersection(PickRayOrigin, PickRayDirecton, HitDistance);

	return(InterSected);
}

// 캐릭터 컨트롤러에 사용하는 함수이다.
// 각 이동 bool 변수와 하나의 키에 대응한다.
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

// 행렬과 쉐이더 및 색상 관련 값들을 쉐이더에 전달한다. Render함수를 실행하면 이 함수도 실행된다. 즉, 직접 사용할 일이 없다.
void GameObject::PrepareRender() {
	SetCamera();

	ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&RotateMatrix), XMLoadFloat4x4(&TranslateMatrix));
	ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ScaleMatrix), ResultMatrix);
	//ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&AnimationMatrix), ResultMatrix);

	// 이미지 출력 모드일경우 종횡비를 적용한다.
	if (RenderType == RENDER_TYPE_2D || RenderType == RENDER_TYPE_2D_STATIC)
		ResultMatrix = XMMatrixMultiply(XMLoadFloat4x4(&ImageAspectMatrix), ResultMatrix);

	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(ResultMatrix));

	RCUtil::Input(GlobalCommandList, &xmf4x4World, GAME_OBJECT_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &ObjectColor, GAME_OBJECT_INDEX, 3, 16);
	RCUtil::Input(GlobalCommandList, &ObjectAlpha, GAME_OBJECT_INDEX, 1, 19);
}

// 렌더링 전 카메라를 설정한다.
void GameObject::SetCamera() {
	// 카메라 뷰 행렬을 설정한다.
	camera.SetViewMatrix();

	// 렌더 타입에 따라 다른 행렬을 초기화 한다.
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

// 피킹 시 사용하는 함수이다. 프로그래머가 이 함수를 직접 사용할 일은 없다.
void GameObject::GenPickingRay(XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, XMVECTOR& PickRayOrigin, XMVECTOR& PickRayDirection) {
	XMMATRIX MatrixTomodel = XMMatrixInverse(NULL, PickMatrix * ViewMatrix);
	XMFLOAT3 CameraOrigin(0.0f, 0.0f, 0.0f);
	PickRayOrigin = XMVector3TransformCoord(XMLoadFloat3(&CameraOrigin), MatrixTomodel);
	PickRayDirection = XMVector3TransformCoord(PickPosition, MatrixTomodel);
	PickRayDirection = XMVector3Normalize(PickRayDirection - PickRayOrigin);
}