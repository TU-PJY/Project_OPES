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
	// ������Ʈ ��ŷ �� ��� ���� ���
	XMMATRIX PickMatrix{};

	// �Ž� ����
	XMFLOAT3 ObjectColor{};

	// �ؽ�ó ����
	float ObjectAlpha{ 1.0f };

	// ������ Ÿ��, �ش� ������ Ÿ�Կ� ���� ������ ������ �޶�����.
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
	// �Ʒ� �Լ����� ��� ��ü���� Ŀ���� ������ ���߾� �Լ����̴�. �ʿ��ϴٸ� ���ο� ���߾� �Լ��� �ۼ��Ͽ� ����� �� �ִ�.
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

	// ����� ���� ���� �Լ��� �Ʒ��� �����Ѵ�.
	
	// ũ�ν���� �ݵ� �ο� �Լ�
	virtual void InputRecoil(float Value) {}

	// ũ�ν���� ������ Ȱ��ȭ/��Ȱ��ȭ
	virtual void EnableRender() {}
	virtual void DisableRender() {}

	// �� �� oobb ��� �Լ�
	virtual std::vector<OOBB> GetMapWallOOBB() { return{}; }

	// ���� �Լ�
	virtual bool CheckHit(XMFLOAT2& checkPosition, int damage) { return{}; }
	virtual void ChangeHP(int hp) {}
	virtual int GetID() { return {}; }
	virtual bool GetDeathState() { return {}; }

	// HP �ε������� �Լ�
	virtual void InputPosition(XMFLOAT3& inputPos, float heightOffset) {}
	virtual void InputHP(int fullHP, int currentHP) {}


	// ���� �׽�Ʈ
	virtual void InputPosition(XMFLOAT3& value) {}
	virtual void InputRotation(XMFLOAT3& value) {}
	virtual void InputState(unsigned int state) {}
};