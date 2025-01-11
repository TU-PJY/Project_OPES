#pragma once
#include "Config.h"
#include "DirectX_3D.h"

class Camera {
protected:
	XMFLOAT3						Position{};

	XMFLOAT3						Right{ 1.0, 0.0, 0.0 };
	XMFLOAT3						Up{ 0.0, 1.0, 0.0 };
	XMFLOAT3						Look{ 0.0, 0.0, 1.0 };

	float           				Pitch{};
	float           				Roll{};
	float           				Yaw{};

	XMFLOAT3						LookAt{};
	XMFLOAT3						Offset{};
	float           				MovingDelay{};

#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	BoundingFrustum					FrustumView{};
	BoundingFrustum					FrustumWorld{};
	XMFLOAT4X4						InverseView{};
#else
	XMFLOAT4						FrustumPlane[6]{};
#endif

	D3D12_RECT						ScissorRect{};

private:
	CamMode Mode{};
	bool StaticMode{};

public:
	D3D12_VIEWPORT					Viewport{};
	XMFLOAT4X4						ViewMatrix{};
	XMFLOAT4X4						ProjectionMatrix{};

	// 정적 출력을 위한 스테틱 행렬 (예: UI, 이미지 등
	XMFLOAT4X4 StaticViewMatrix{};
	XMFLOAT4X4 StaticProjectionMatrix{};

	Camera();
	virtual ~Camera() {}
	void Update(float FT);

	virtual void CreateShaderVariables(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList) {}
	virtual void ReleaseShaderVariables() {}
	void SetToDefaultMode();

	void SetToStaticMode();
	void UpdateShaderVariables();
	void SetViewMatrix();
	void InitStaticMatrix();
	void GenerateStaticMatrix();
	void GeneratePerspectiveMatrix(float NearPlane, float FarPlane, float AspRatio, float Fov);
	void GenerateOrthoMatrix(float Width, float Height, float AspRatio, float Near, float Far);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float zMin = 0.0f, float zMax = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	void SetViewportsAndScissorRects();
	void SwitchCameraMode(CamMode GetMode);

	CamMode CurrentMode();

	void Move(XMFLOAT3 PositionValue);
	XMFLOAT3& GetPosition();

	void Rotate(float X, float Y, float Z);


	void SetLookAtPosition(XMFLOAT3 LookAtValue);
	XMFLOAT3& GetLookAtPosition();

	XMFLOAT3& GetRightVector();
	XMFLOAT3& GetUpVector();
	XMFLOAT3& GetLookVector();

	float& GetPitch();
	float& GetRoll();
	float& GetYaw();

	void SetOffset(XMFLOAT3 Value);
	XMFLOAT3& GetOffset();

	void SetTimeLag(float DelayValue);
	float GetTimeLag();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();
	D3D12_VIEWPORT GetViewport();
	D3D12_RECT GetScissorRect();

	void Move(float X, float Y, float Z);
	void Vector_MoveForward(float MoveDistance);
	void MoveForward(float MoveDistance);
	void Vector_MoveStrafe(float MoveDistance);
	void Vector_MoveUp(XMFLOAT3& Position, XMFLOAT3 Up, float Distance);
	void MoveStrafe(float MoveDistance);
	void MoveUp(float MoveDistance);
	void Track(XMFLOAT3& ObjectPosition, Vector& VectorStruct, float fTimeElapsed);
	void TrackOffset(XMFLOAT3& ObjectPosition, Vector& VectorStruct, XMFLOAT3& OffsetValue, float fTimeElapsed);
	void SetLookAt(XMFLOAT3& ObjectPosition, XMFLOAT3& UpVec);
	void CalculateFrustumPlanes();
	bool IsInFrustum(BoundingBox& BoundingBox);
	bool IsInFrustum(BoundingOrientedBox& BoundingBox);
};

extern Camera camera;