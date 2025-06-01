#pragma once
#include "GameResource.h"
#include "CBVUtil.h"
#include "TransformUtil.h"
#include "CollisionUtil.h"
#include "MathUtil.h"
#include "LineBrush.h"
#include "TerrainUtil.h"
#include "Matrix.h"

class Shader;
typedef ID3D12GraphicsCommandList* (CommandList);

class GameObject {
public:
	// 오브젝트 피킹 될 경우 사용될 행렬
	XMMATRIX PickMatrix{};

	// 매쉬 색상
	XMFLOAT3 ObjectColor{};

	// 텍스처 투명도
	float ObjectAlpha{ 1.0f };

	// 렌더링 타입, 해당 렌더링 타입에 따라 렌더링 형식이 달라진다.
	int RenderType{ RENDER_TYPE_3D };

	int ObjectLayer{};
	std::string ObjectTag{};
	bool DeleteCommand{};

	void BeginRender(int RenderTypeFlag = RENDER_TYPE_3D);
	void SetColor(XMFLOAT3& Color);
	void SetColor(float R, float G, float B);
	void SetColorRGB(float R, float G, float B);
	void SetLightUse(int Flag);
	void SetFogUse(int Flag);
	void FlipTexture(int FlipType);
	float ASP(float Value);
	void UpdateFBXAnimation(FBXMesh& TargetMesh, float Time);
	void SelectFBXAnimation(FBXMesh& TargetMesh, std::string AnimationName);
	void ResetAnimationTime(FBXMesh& TargetMesh);
	void RenderFBX(FBXMesh& TargetMesh, Texture* TexturePtr, float AlphaValue = 1.0, int DepthTestFlag = DEPTH_TEST_DEFAULT);
	void RenderFBX(FBX& TargetFBX, Texture* TexturePtr, float AlphaValue = 1.0, int DepthTestFlag = DEPTH_TEST_DEFAULT);
	int PickRayFBX(FBXMesh& TargetMesh, XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, float* HitDistance);
	void Render3D(Mesh* MeshPtr, Texture* TexturePtr, float AlphaValue=1.0f, int DepthTestFlag=DEPTH_TEST_DEFAULT);
	void Render2D(Texture* TexturePtr, float AlphaValue=1.0f, bool EnableAspect=true);
	void UpdateMotionRotation(float& RotationX, float& RotationY, float DeltaX, float DeltaY);
	void UpdateMotionRotation(XMFLOAT3& Rotation, float DeltaX, float DeltaY);
	void UpdatePickMatrix();
	int PickRayInter(Mesh* MeshPtr, XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, float* HitDistance);
	void InputBoolSwitch(int SwitchFlag, KeyEvent& Event, WPARAM Key, bool& BoolValue);

private:
	void PrepareRender();
	void SetCamera();
	void GenPickingRay(XMVECTOR& PickPosition, XMMATRIX& ViewMatrix, XMVECTOR& PickRayOrigin, XMVECTOR& PickRayDirection);

	////////// virtual functions
public:
	// 아래 함수들은 모든 객체에서 커스텀 가능한 버추얼 함수들이다. 필요하다면 새로운 버추얼 함수를 작성하여 사용할 수 있다.
	virtual ~GameObject() {}
	virtual void ReleaseShaderVariables() {}
	virtual void InputKey(KeyEvent& Event) {}
	virtual void InputMouse(MouseEvent& Event) {}
	virtual void InputMouseMotion(MotionEvent& Event) {}
	virtual void Update(float FT) {}
	virtual void Render() {}
	virtual Mesh* GetObjectMesh() { return {}; }
	virtual XMFLOAT3 GetPosition() { return {}; }
	virtual AABB GetAABB() { return {}; }
	virtual OOBB GetOOBB() { return {}; }
	virtual BoundSphere GetBoundSphere() { return {}; }
	virtual Vector GetVectorSet() { return {}; }
	virtual TerrainUtil GetTerrain() { return {}; }

	// 사용자 정의 리턴 함수는 아래에 정의한다.
	
	// 크로스헤어 반동 부여 함수
	virtual void InputRecoil(float Value) {}

	// 크로스헤어 렌더링 활성화/비활성화
	virtual void EnableRender() {}
	virtual void DisableRender() {}

	// 맵 벽 oobb 얻는 함수
	virtual std::vector<OOBB> GetMapWallOOBB() { return{}; }

	// 몬스터 함수
	virtual bool CheckHit(XMFLOAT2& checkPosition, int damage) { return{}; }
	virtual void ChangeHP(int hp) {}
	virtual int GetID() { return {}; }
	virtual bool GetDeathState() { return {}; }

	// HP 인디케이터 함수
	virtual void InputPosition(XMFLOAT3& inputPos, float heightOffset) {}
	virtual void InputHP(int fullHP, int currentHP) {}


	// 서버 테스트
	virtual void InputPosition(XMFLOAT3& value) {}
	virtual void InputRotation(XMFLOAT3& value) {}
	virtual void InputState(unsigned int state) {}
};