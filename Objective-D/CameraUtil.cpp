#include "CameraUtil.h"
#include "RootConstants.h"
#include "RootConstantUtil.h"
#include "TransformUtil.h"
#include "MathUtil.h"
#include "RandomUtil.h"
#include <numeric>

static float Lerp(float a, float b, float t) { return a + (b - a) * t; }
static float Fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); } // Perlin의 quintic
static int   Perm[512]; // 시드로 채우는 순열 테이블

static void InitPerm(uint32_t seed) {
	std::mt19937 rng(seed);
	std::vector<int> p(256);
	std::iota(p.begin(), p.end(), 0);
	std::shuffle(p.begin(), p.end(), rng);
	for (int i = 0; i < 512; ++i) Perm[i] = p[i & 255];
}
static float Grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y;
	float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
static float Perlin3(float x, float y, float z) {
	int X = (int)floorf(x) & 255;
	int Y = (int)floorf(y) & 255;
	int Z = (int)floorf(z) & 255;
	x -= floorf(x); y -= floorf(y); z -= floorf(z);
	float u = Fade(x), v = Fade(y), w = Fade(z);
	int A = Perm[X] + Y, AA = Perm[A] + Z, AB = Perm[A + 1] + Z;
	int B = Perm[X + 1] + Y, BA = Perm[B] + Z, BB = Perm[B + 1] + Z;
	float res =
		Lerp(Lerp(Lerp(Grad(Perm[AA], x, y, z),
			Grad(Perm[BA], x - 1, y, z), u),
			Lerp(Grad(Perm[AB], x, y - 1, z),
				Grad(Perm[BB], x - 1, y - 1, z), u), v),
			Lerp(Lerp(Grad(Perm[AA + 1], x, y, z - 1),
				Grad(Perm[BA + 1], x - 1, y, z - 1), u),
				Lerp(Grad(Perm[AB + 1], x, y - 1, z - 1),
					Grad(Perm[BB + 1], x - 1, y - 1, z - 1), u), v), w);
	// 결과 범위 대략 [-1,1]
	return res;
}

struct ShakeParams {
	float duration = 0.35f;     // 전체 길이
	float freqPos = 10.0f;     // 위치 노이즈 주파수(Hz)
	float freqRot = 8.0f;      // 회전 노이즈 주파수(Hz)
	xmfloat3  ampPos = { 0.02f, 0.02f, 0.01f }; // m 또는 world units
	xmfloat3  ampRot = { 0.6f, 0.8f, 0.3f };    // degrees
	float decayK = 6.0f;      // 지수 감쇠 세기 (커질수록 빨리 죽음)
	uint32_t seed = 12345;     // 시드(무기마다 다르게 주면 패턴 차별화)
	float   phase = 0.0f;      // 시간 오프셋(쇼트마다 랜덤 추가)
	float   distanceFalloffK = 0.06f; // 거리 감쇠 계수
};

struct ShakeState {
	bool   active = false;
	float  t = 0.0f;     // 누적 시간(초)
	float  life = 0.0f;  // 남은 시간
	ShakeParams P;
};
ShakeState gShake;

static float Attenuation(float d, float k) { return 1.0f / (1.0f + k * d * d); }

inline void StartShake(ShakeState& S, const ShakeParams& P, float distanceToEvent = 0.0f) {
	S.P = P;
	S.P.phase = std::uniform_real_distribution<float>(0.0f, 1000.0f)(std::mt19937(P.seed)); // 임의 위상
	InitPerm(P.seed); // 시드 기반 노이즈 테이블
	S.t = 0.0f;
	S.life = P.duration;
	S.active = true;

	// 거리 감쇠를 시작 세기에 반영하고 싶다면, 바로 amp에 곱하거나 별도 스케일 유지
	float att = Attenuation(distanceToEvent, P.distanceFalloffK);
	S.P.ampPos.x *= att; S.P.ampPos.y *= att; S.P.ampPos.z *= att;
	S.P.ampRot.x *= att; S.P.ampRot.y *= att; S.P.ampRot.z *= att;
}

inline void UpdateShake(ShakeState& S, float dt, xmfloat3& outPos, xmfloat3& outRotDeg) {
	outPos = { 0,0,0 };
	outRotDeg = { 0,0,0 };
	if (!S.active) return;

	S.t += dt;
	S.life -= dt;
	float time = S.t + S.P.phase;

	// 지수 감쇠(0~1)
	float env = expf(-S.P.decayK * (S.t / std::max(0.0001f, S.P.duration)));
	env = std::clamp(env, 0.0f, 1.0f);

	// 축별 샘플 좌표(서로 다른 위상/시드를 주려면 상수 오프셋)
	float px = Perlin3(time * S.P.freqPos, 3.17f, 8.23f);
	float py = Perlin3(5.91f, time * S.P.freqPos, 11.71f);
	float pz = Perlin3(9.73f, 6.41f, time * S.P.freqPos);

	float rx = Perlin3(time * S.P.freqRot, 2.11f, 4.77f);
	float ry = Perlin3(7.33f, time * S.P.freqRot, 1.19f);
	float rz = Perlin3(0.41f, 5.55f, time * S.P.freqRot);

	// [-1,1] → 곱
	outPos.x = px * S.P.ampPos.x * env;
	outPos.y = py * S.P.ampPos.y * env;
	outPos.z = pz * S.P.ampPos.z * env;

	outRotDeg.x = rx * S.P.ampRot.x * env; // pitch
	outRotDeg.y = ry * S.P.ampRot.y * env; // yaw
	outRotDeg.z = rz * S.P.ampRot.z * env; // roll

	if (S.life <= 0.0f) S.active = false;
}

// Config.h 에서 작성한 모드에 따라 카메라가 다르게 동작하도록 작성할 수 있다.
// 예) 카메라 추적 대상 변경, 카메라 시점 변경 등
void Camera::Update(float FT) {
	// 카메라 흔들림
	// ShakeStrength를 기반으로 실시간으로 랜덤 값을 생성하여 뷰포트 오프셋을 이동시키며, 흔들림 강도는 선형 보간으로 감소한다.
	// 너무 빠르게 흔들리는것을 방지하기 위해 딜레이를 준다.
	ShakeDelay += FT;
	if (ShakeDelay >= 0.01) {
		ShakeStrength = std::lerp(ShakeStrength, 0.0, 5.0 * FT);
		ShakeOffset.x = Random.Gen(-ShakeStrength, ShakeStrength);
		ShakeOffset.y = Random.Gen(-ShakeStrength, ShakeStrength);
		ShakeDelay -= 0.01;
	}

	xmfloat3 shakePos, shakeRotDeg;
	UpdateShake(gShake, FT, shakePos, shakeRotDeg);
	recoilShakeNum = std::lerp(recoilShakeNum, shakeRotDeg.z, 10.0 * FT);
	recoilShake = std::lerp(recoilShake, recoilShakeNum, 20.0 * FT);
}

void Camera::SetShake(float Strength) {
	ShakeStrength = Strength;
}

void Camera::AddShake(float Strength) {
	if (Strength <= 0.0)
		return;

	ShakeStrength += Strength;
}

void Camera::AddRecoilShake(float Strength) {
	ShakeParams params{};
	params.freqRot = 30.0f;
	params.duration = 0.3f;
	
	int randNum = Random.Gen(0, 1);
	if (randNum == 1)
		params.ampRot = { 0.0f, 0.0f, Strength }; // 반동 세기
	else
		params.ampRot = { 0.0f, 0.0f, -Strength }; // 반동 세기
	StartShake(gShake, params);
}

void Camera::AddShake(float d, float R0, float R1, float p, float Strength) {
	if (d <= R0) {
		ShakeStrength += Strength;
		return;
	}
	if (d >= R1) 
		return;

	float t = (d - R0) / (R1 - R0);
	ShakeStrength += powf(1.0f - t, p) * Strength;
}

void Camera::AddShakeWithDistance(float Distance, float Strength) {
	float t = (15.0 - Distance) / 15.0;
	t = std::clamp(t, 0.0f, 1.0f);
	int damage = (int)(300.0 * t);
}

// 카메라 모드를 변경한다. Config.h에 작성했던 모드 열거형을 파라미터에 넣으면 된다.
void Camera::SwitchCameraMode(CamMode ModeValue) {
	Mode = ModeValue;
}

// 현재 실행 중인 카메라 모드를 얻는다.
CamMode Camera::CurrentMode() {
	return Mode;
}

// 정적 출력 모드로 전환한다.
void Camera::SetToStaticMode() {
	StaticMode = true;
}

// 일반 출력 모드로 전환한다.
void Camera::SetToDefaultMode() {
	StaticMode = false;
}



Camera::Camera() {
	ViewMatrix = Mat4::Identity();
	ProjectionMatrix = Mat4::Identity();

	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	Pitch = 0.0f;
	Roll = 0.0f;
	Yaw = 0.0f;

	Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	Up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	LookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);

	MovingDelay = 0.0f;
}



void Camera::UpdateShaderVariables() {
	XMFLOAT4X4 xmf4x4View;
	XMFLOAT4X4 xmf4x4Projection;

	// 스테틱 모드 실행 시 스테틱 행렬을 쉐이더로 전달한다.
	switch(StaticMode) {
	case false:
		ProjectionMatrix._31 += ShakeOffset.x;
		ProjectionMatrix._32 += ShakeOffset.y;

		XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&ViewMatrix)));
		XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&ProjectionMatrix)));
		RCUtil::Input(GlobalCommandList, &Position, CAMERA_INDEX, 3, 32);
		break;

	case true:
		XMFLOAT3 StaticPosition{ 0.0, 0.0, 0.0 };
		XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&StaticViewMatrix)));
		XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&StaticProjectionMatrix)));
		RCUtil::Input(GlobalCommandList, &StaticPosition, CAMERA_INDEX, 3, 32);
	}

	RCUtil::Input(GlobalCommandList, &xmf4x4View, CAMERA_INDEX, 16, 0);
	RCUtil::Input(GlobalCommandList, &xmf4x4Projection, CAMERA_INDEX, 16, 16);
}

// 카메라 뷰 행렬을 설정한다.
void Camera::SetViewMatrix() {
	Look = Vec3::Normalize(Look);
	Right = Vec3::CrossProduct(Up, Look, true);
	Up = Vec3::CrossProduct(Look, Right, true);

	ViewMatrix._11 = Right.x; ViewMatrix._12 = Up.x; ViewMatrix._13 = Look.x;
	ViewMatrix._21 = Right.y; ViewMatrix._22 = Up.y; ViewMatrix._23 = Look.y;
	ViewMatrix._31 = Right.z; ViewMatrix._32 = Up.z; ViewMatrix._33 = Look.z;

	ViewMatrix._41 = -Vec3::DotProduct(Position, Right);
	ViewMatrix._42 = -Vec3::DotProduct(Position, Up);
	ViewMatrix._43 = -Vec3::DotProduct(Position, Look);

	InverseView._11 = Right.x; InverseView._12 = Right.y; InverseView._13 = Right.z;
	InverseView._21 = Up.x; InverseView._22 = Up.y; InverseView._23 = Up.z;
	InverseView._31 = Look.x; InverseView._32 = Look.y; InverseView._33 = Look.z;
	InverseView._41 = Position.x; InverseView._42 = Position.y; InverseView._43 = Position.z;

	FrustumView.Transform(FrustumWorld, XMLoadFloat4x4(&InverseView));
	CalculateFrustumPlanes();
}

void Camera::SetStaticViewMatrix() {
	StaticViewMatrix._11 = StaticRight.x; StaticViewMatrix._12 = StaticUp.x; StaticViewMatrix._13 = StaticLook.x;
	StaticViewMatrix._21 = StaticRight.y; StaticViewMatrix._22 = StaticUp.y; StaticViewMatrix._23 = StaticLook.y;
	StaticViewMatrix._31 = StaticRight.z; StaticViewMatrix._32 = StaticUp.z; StaticViewMatrix._33 = StaticLook.z;

	StaticViewMatrix._41 = -Vec3::DotProduct(XMFLOAT3(0.0, 0.0, 0.0), StaticRight);
	StaticViewMatrix._42 = -Vec3::DotProduct(XMFLOAT3(0.0, 0.0, 0.0), StaticUp);
	StaticViewMatrix._43 = -Vec3::DotProduct(XMFLOAT3(0.0, 0.0, 0.0), StaticLook);
}

// 원근 투영 행렬을 초기화한다. 윈도우 사이즈 변경 시 이 함수가 실행된다.
void Camera::GeneratePerspectiveMatrix(float NearPlane, float FarPlane, float AspRatio, float Fov) {
	Transform::Identity(ProjectionMatrix);
	XMMATRIX Projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(Fov), AspRatio, NearPlane, FarPlane);
	XMStoreFloat4x4(&ProjectionMatrix, Projection);

#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	BoundingFrustum::CreateFromMatrix(FrustumView, Projection);
	//FrustumView.Transform(FrustumWorld, XMLoadFloat4x4(&InverseView));
#endif
}

// 직각 투영 행렬을 초기화 한다
void Camera::GenerateOrthoMatrix(float Width, float Height, float AspRatio, float Near, float Far) {
	Transform::Identity(ProjectionMatrix);
	XMMATRIX Projection = XMMatrixOrthographicLH(Width * AspRatio * 2.0, Height * 2.0, Near, Far);
	XMStoreFloat4x4(&ProjectionMatrix, Projection);

#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	//BoundingFrustum::CreateFromMatrix(FrustumView, Projection);
#endif
}

// 정적 직각 투영 행렬을 초기화 한다
void Camera::GenerateStaticMatrix() {
	Transform::Identity(StaticViewMatrix);
	Transform::Identity(StaticProjectionMatrix);
	XMMATRIX Projection = XMMatrixOrthographicLH(2.0 * ASPECT, 2.0, 0.0, 10.0);
	XMStoreFloat4x4(&StaticProjectionMatrix, Projection);
}

// 정적 출력을 위한 스테틱 행렬을 생성한다. UI, 이미지 등의 출력을 목적으로 하는 행렬이므로 
// 프로그램 실행 시 최초 1회만 실행한다.
// 직각투영이 디폴트이다.
void Camera::InitStaticMatrix() {
	Transform::Identity(StaticViewMatrix);
	Transform::Identity(StaticProjectionMatrix);
	XMMATRIX Projection = XMMatrixOrthographicLH(2.0 * ASPECT, 2.0, 0.0, 10.0);
	XMStoreFloat4x4(&StaticProjectionMatrix, Projection);
}

// 뷰포트를 설정한다. 한 번 설정한 이후에는 건들 필요 없다.
void Camera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float zMin, float zMax) {
	Viewport.TopLeftX = float(xTopLeft);
	Viewport.TopLeftY = float(yTopLeft);
	Viewport.Width = float(nWidth);
	Viewport.Height = float(nHeight);
	Viewport.MinDepth = zMin;
	Viewport.MaxDepth = zMax;
}

// 시저 렉트를 설정한다. 한 번 설정한 이후에는 건들 필요 없다.
void Camera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom) {
	ScissorRect.left = xLeft;
	ScissorRect.top = yTop;
	ScissorRect.right = xRight;
	ScissorRect.bottom = yBottom;
}

// 뷰포트와 시저렉트를 쉐이더로 전달한다.
void Camera::SetViewportsAndScissorRects() {
	GlobalCommandList->RSSetViewports(1, &Viewport);
	GlobalCommandList->RSSetScissorRects(1, &ScissorRect);
}



// 위치 이동, 시점 추적 위치 설정 등 회전각도, 위치, 벡터 관련 함수들이다.
XMFLOAT3& Camera::GetPosition() { 
	return Position; 
}

void Camera::SetLookAtPosition(XMFLOAT3 LookAtValue) { 
	LookAt = LookAtValue; 
}

XMFLOAT3& Camera::GetLookAtPosition() { 
	return LookAt; 
}

XMFLOAT3& Camera::GetRightVector() { 
	return Right; 
}

XMFLOAT3& Camera::GetUpVector() {
	return Up; 
}

XMFLOAT3& Camera::GetLookVector() { 
	return Look; 
}

float& Camera::GetPitch() { 
	return Pitch; 
}

float& Camera::GetRoll() {
	return Roll;
}

float& Camera::GetYaw() { 
	return Yaw;
}

void Camera::SetOffset(XMFLOAT3 Value) { 
	Offset = Value;
}

XMFLOAT3& Camera::GetOffset() { 
	return Offset;
}

void Camera::SetTimeLag(float DelayValue) { 
	MovingDelay = DelayValue;
}

float Camera::GetTimeLag() { 
	return MovingDelay;
}

XMFLOAT4X4 Camera::GetViewMatrix() { 
	return ViewMatrix; 
}

XMFLOAT4X4 Camera::GetProjectionMatrix() {
	return ProjectionMatrix; 
}

D3D12_VIEWPORT Camera::GetViewport() { 
	return Viewport; 
}

D3D12_RECT Camera::GetScissorRect() { 
	return ScissorRect; 
}



// 카메라의 위치를 변경한다.
void Camera::Move(float X, float Y, float Z) {
	Position.x = X;
	Position.y = Y;
	Position.z = Z;
}

// 카메라의 위치를 변경한다.
void Camera::Move(XMFLOAT3 PositionValue) { 
	Position = PositionValue; 
}

// 현재 시점에서 look 벡터를 사용하여 앞으로 움직인다.
void Camera::Vector_MoveForward(float MoveDistance) {
	XMFLOAT3 NormlaizedLook = Vec3::Normalize(Look);

	Position.x += NormlaizedLook.x * MoveDistance;
	Position.y += NormlaizedLook.y * MoveDistance;
	Position.z += NormlaizedLook.z * MoveDistance;
}

// 현재 시점에서 right 벡터를 사용하여 옆으로 움직인다.
void Camera::Vector_MoveStrafe(float MoveDistance) {
	XMFLOAT3 NormlaizedRight = Vec3::Normalize(Right);

	Position.x += NormlaizedRight.x * MoveDistance;
	Position.y += NormlaizedRight.y * MoveDistance;
	Position.z += NormlaizedRight.z * MoveDistance;
}

// 현재 시점에서 up 벡터를 사용하여 위로 움직인다.
void Camera::Vector_MoveUp(XMFLOAT3& Position, XMFLOAT3 Up, float Distance) {
	Position = Vec3::Add(Position, Up, Distance);
}

// 현재 시점에서 앞으로 움직인다.
void Camera::MoveForward(float MoveDistance) {
	Position.x += sin(Yaw) * MoveDistance;
	Position.z += cos(Yaw) * MoveDistance;
}

// 현재 시점에서 옆으로 움직인다.
void Camera::MoveStrafe(float MoveDistance) {
	Position.x += cos(Yaw) * MoveDistance;
	Position.z -= sin(Yaw) * MoveDistance;
}

// 수직으로 움직인다.
void Camera::MoveUp(float MoveDistance) {
	Position.y += MoveDistance;
}

// 카메라 회전, 새로운 벡터를 지정한다.
void Camera::Rotate(float X, float Y, float Z) {
	Look = XMFLOAT3(0.0, 0.0, 1.0);
	Right = XMFLOAT3(1.0, 0.0, 0.0);
	Up = XMFLOAT3(0.0, 1.0, 0.0);

	Pitch = XMConvertToRadians(X);
	Yaw = XMConvertToRadians(Y);
	Roll = XMConvertToRadians(Z);

	// 회전 행렬 생성
	XMMATRIX RotationMatrix = XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll);

	XMStoreFloat3(&Look, XMVector3TransformNormal(XMLoadFloat3(&Look), RotationMatrix));
	XMStoreFloat3(&Right, XMVector3TransformNormal(XMLoadFloat3(&Right), RotationMatrix));
	XMStoreFloat3(&Up, XMVector3TransformNormal(XMLoadFloat3(&Up), RotationMatrix));
}

// 파라미터로 입력받은 위치, 업벡터, 라이트벡터, 룩벡터를 추적한다.
void Camera::Track(XMFLOAT3& ObjectPosition, Vector& VectorStruct, float fTimeElapsed) {
	XMFLOAT4X4 RotateMatrix = Mat4::Identity();

	XMFLOAT3 UpVector = VectorStruct.Up;
	XMFLOAT3 RightVector = VectorStruct.Right;
	XMFLOAT3 LookVector = VectorStruct.Look;

	RotateMatrix._21 = UpVector.x;
	RotateMatrix._22 = UpVector.y;
	RotateMatrix._23 = UpVector.z;

	RotateMatrix._11 = RightVector.x;
	RotateMatrix._12 = RightVector.y;
	RotateMatrix._13 = RightVector.z;

	RotateMatrix._31 = LookVector.x;
	RotateMatrix._32 = LookVector.y;
	RotateMatrix._33 = LookVector.z;

	XMFLOAT3 Direction = Vec3::Subtract(Vec3::Add(ObjectPosition, Vec3::TransformCoord(Offset, RotateMatrix)), Position);

	float Length = Vec3::Length(Direction);
	Direction = Vec3::Normalize(Direction);

	float TimeLagScale = (MovingDelay) ? fTimeElapsed * (1.0f / MovingDelay) : 1.0f;
	float MoveDistance = Length * TimeLagScale;

	if (MoveDistance > Length)
		MoveDistance = Length;

	if (Length < 0.01f)
		MoveDistance = Length;

	Position = Vec3::Add(Position, Direction, MoveDistance);
	SetLookAt(ObjectPosition, VectorStruct.Up);
}

// 동작은 Track과 동일하나, 시점 Offset을 설정할 수 있다.
void Camera::TrackOffset(XMFLOAT3& ObjectPosition, Vector& VectorStruct, XMFLOAT3& OffsetValue, float fTimeElapsed) {
	XMFLOAT4X4 RotateMatrix = Mat4::Identity();

	XMFLOAT3 UpVector = VectorStruct.Up;
	XMFLOAT3 RightVector = VectorStruct.Right;
	XMFLOAT3 LookVector = VectorStruct.Look;

	RotateMatrix._21 = UpVector.x;
	RotateMatrix._22 = UpVector.y;
	RotateMatrix._23 = UpVector.z;

	RotateMatrix._11 = RightVector.x;
	RotateMatrix._12 = RightVector.y;
	RotateMatrix._13 = RightVector.z;

	RotateMatrix._31 = LookVector.x;
	RotateMatrix._32 = LookVector.y;
	RotateMatrix._33 = LookVector.z;

	XMFLOAT3 Direction = Vec3::Subtract(Vec3::Add(ObjectPosition, Vec3::TransformCoord(Offset, RotateMatrix)), Position);

	float Length = Vec3::Length(Direction);
	Direction = Vec3::Normalize(Direction);

	float TimeLagScale = (MovingDelay) ? fTimeElapsed * (1.0f / MovingDelay) : 1.0f;
	float MoveDistance = Length * TimeLagScale;

	if (MoveDistance > Length)
		MoveDistance = Length;

	if (Length < 0.01f)
		MoveDistance = Length;

	Position = Vec3::Add(Position, Direction, MoveDistance);

	// 로컬 좌표계에서 LookAtPosition 조정
	XMFLOAT3 LookAtPosition = ObjectPosition;

	// 로컬 좌표계를 기준으로 오프셋 적용
	LookAtPosition = Vec3::Add(LookAtPosition, Vec3::Scale(VectorStruct.Right, OffsetValue.x));
	LookAtPosition = Vec3::Add(LookAtPosition, Vec3::Scale(VectorStruct.Up, OffsetValue.y));
	LookAtPosition = Vec3::Add(LookAtPosition, Vec3::Scale(VectorStruct.Look, OffsetValue.z));

	SetLookAt(LookAtPosition, VectorStruct.Up);
}

// 카메라가 바라보는 방향을 설정한다. Track에서 실행되므로 보통의 경우 직접 쓸 일은 없다.
void Camera::SetLookAt(XMFLOAT3& ObjectPosition, XMFLOAT3& UpVec) {
	XMFLOAT4X4 mtxLookAt = Mat4::LookAtLH(Position, ObjectPosition, UpVec);
	Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}

// 프러스텀 관련 함수들
void Camera::CalculateFrustumPlanes() {
#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	FrustumView.Transform(FrustumWorld, XMMatrixInverse(NULL, XMLoadFloat4x4(&ViewMatrix)));
#else
	XMFLOAT4X4 CamViewProjection = Mat4::Multiply(ViewMatrix, ProjectionMatrix);

	FrustumPlane[0].x = -(CamViewProjection._14 + CamViewProjection._11);
	FrustumPlane[0].y = -(CamViewProjection._24 + CamViewProjection._21);
	FrustumPlane[0].z = -(CamViewProjection._34 + CamViewProjection._31);
	FrustumPlane[0].w = -(CamViewProjection._44 + CamViewProjection._41);

	FrustumPlane[1].x = -(CamViewProjection._14 - CamViewProjection._11);
	FrustumPlane[1].y = -(CamViewProjection._24 - CamViewProjection._21);
	FrustumPlane[1].z = -(CamViewProjection._34 - CamViewProjection._31);
	FrustumPlane[1].w = -(CamViewProjection._44 - CamViewProjection._41);

	FrustumPlane[2].x = -(CamViewProjection._14 - CamViewProjection._12);
	FrustumPlane[2].y = -(CamViewProjection._24 - CamViewProjection._22);
	FrustumPlane[2].z = -(CamViewProjection._34 - CamViewProjection._32);
	FrustumPlane[2].w = -(CamViewProjection._44 - CamViewProjection._42);

	FrustumPlane[3].x = -(CamViewProjection._14 + CamViewProjection._12);
	FrustumPlane[3].y = -(CamViewProjection._24 + CamViewProjection._22);
	FrustumPlane[3].z = -(CamViewProjection._34 + CamViewProjection._32);
	FrustumPlane[3].w = -(CamViewProjection._44 + CamViewProjection._42);

	FrustumPlane[4].x = -(CamViewProjection._13);
	FrustumPlane[4].y = -(CamViewProjection._23);
	FrustumPlane[4].z = -(CamViewProjection._33);
	FrustumPlane[4].w = -(CamViewProjection._43);

	FrustumPlane[5].x = -(CamViewProjection._14 - CamViewProjection._13);
	FrustumPlane[5].y = -(CamViewProjection._24 - CamViewProjection._23);
	FrustumPlane[5].z = -(CamViewProjection._34 - CamViewProjection._33);
	FrustumPlane[5].w = -(CamViewProjection._44 - CamViewProjection._43);

	for (int i = 0; i < 6; i++)
		FrustumPlane[i] = Plane::Normalize(FrustumPlane[i]);
#endif
}

bool Camera::CheckFrustum(AABB& aabb) {
	return IsInFrustum(aabb.aabb);
}

bool Camera::CheckFrustum(OOBB& oobb) {
	return IsInFrustum(oobb.oobb);
}

bool Camera::CheckFrustum(BoundSphere& Sphere) {
	return IsInFrustum(Sphere.sphere);
}

bool Camera::IsInFrustum(BoundingSphere& Sphere) {
	return(FrustumWorld.Intersects(Sphere));
}

bool Camera::IsInFrustum(BoundingBox& BoundingBox) {
#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	return(FrustumWorld.Intersects(BoundingBox));
#else
	XMFLOAT3 NearPoint, Normal;
	XMFLOAT3 Min = Vec3::Subtract(BoundingBox.Center, BoundingBox.Extents);
	XMFLOAT3 Max = Vec3::Add(BoundingBox.Center, BoundingBox.Extents);
	for (int i = 0; i < 6; i++) {
		Normal = XMFLOAT3(FrustumPlane[i].x, FrustumPlane[i].y, FrustumPlane[i].z);

		if (Normal.x >= 0.0f) {
			if (Normal.y >= 0.0f) {
				if (Normal.z >= 0.0f)
					NearPoint = XMFLOAT3(Min.x, Min.y, Min.z);
				else
					NearPoint = XMFLOAT3(Min.x, Min.y, Max.z);
			}

			else {
				if (Normal.z >= 0.0f)
					NearPoint = XMFLOAT3(Min.x, Max.y, Min.z);
				else
					NearPoint = XMFLOAT3(Min.x, Max.y, Max.z);
			}
		}

		else {
			if (Normal.y >= 0.0f) {
				if (Normal.z >= 0.0f)
					NearPoint = XMFLOAT3(Max.x, Min.y, Min.z);
				else
					NearPoint = XMFLOAT3(Max.x, Min.y, Max.z);
			}

			else {
				if (Normal.z >= 0.0f)
					NearPoint = XMFLOAT3(Max.x, Max.y, Min.z);
				else
					NearPoint = XMFLOAT3(Max.x, Max.y, Max.z);
			}
		}

		if ((Vec3::DotProduct(Normal, NearPoint) + FrustumPlane[i].w) > 0.0f)
			return(false);
	}

	return(true);
#endif
}

bool Camera::IsInFrustum(BoundingOrientedBox& BoundingBox) {
#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	return(FrustumWorld.Intersects(BoundingBox));
#else
#endif
}