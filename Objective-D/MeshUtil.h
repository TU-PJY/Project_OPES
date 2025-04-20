#pragma once
#include "DirectX_3D.h"
#include <fstream>
#include <unordered_map>
#include <set>

struct FBXVertex {
	float px, py, pz;   // Position
	float nx, ny, nz;   // Normal
	float u, v;         // UV
};

struct AnimationKeyFrame {
	float time;
	float translation[3];
	float rotation[3];
	float scale[3];
};

struct AnimationChannel {
	std::string nodeName;
	std::vector<AnimationKeyFrame> keyframes;
};

// 모든 애니메이션 채널을 담을 컨테이너
extern std::vector<AnimationChannel> AnimationChannels;

class Mesh;
extern std::vector<Mesh*> AnimatedMesh;

class Mesh {
private:
	int Ref{};

public:
	void AddRef();
	void Release();

	std::string nodeName{};

	UINT Vertices{};
	XMFLOAT3* Position{};
	ID3D12Resource* PositionBuffer{};
	ID3D12Resource* PositionUploadBuffer{};

	XMFLOAT3* Normal{};
	ID3D12Resource* NormalBuffer{};
	ID3D12Resource* NormalUploadBuffer{};

	XMFLOAT2* TextureCoords{};
	ID3D12Resource* TextureCoordBuffer{};
	ID3D12Resource* TextureCoordUploadBuffer{};

	ID3D12Resource* BoneIndexBuffer{};
	ID3D12Resource* BoneIndexUploadBuffer{};

	ID3D12Resource* BoneWeightBuffer{};
	ID3D12Resource* BoneWeightUploadBuffer{};

	UINT Indices{};
	UINT* PnIndices{};
	ID3D12Resource* IndexBuffer{};
	ID3D12Resource* IndexUploadBuffer{};

	UINT NumVertexBufferViews{};
	D3D12_VERTEX_BUFFER_VIEW* VertexBufferViews{};

	D3D12_INDEX_BUFFER_VIEW	IndexBufferView{};

	D3D12_PRIMITIVE_TOPOLOGY PromitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT Slot{};
	UINT Stride{};
	UINT Offset{};

	bool HeightCacheSaved{};
	std::vector<XMFLOAT3> HeightCache;


public:
	BoundingOrientedBox	OOBB = BoundingOrientedBox();

	Mesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, char* Directory, int Type);
	Mesh();
	~Mesh();
	void ReleaseUploadBuffers();
	void Render(ID3D12GraphicsCommandList* CmdList);
	BOOL RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance);
	int CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance);
	void ClearHeightCache();
	void SetHeightCache(Mesh* terrainMesh, const XMFLOAT4X4& worldMatrix);
	void CreateSkyboxMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList);
	void CreateImagePannelMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList);
	void CreateBoundboxMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList);
	void ImportMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, char* Directory, bool TextMode);
	void CreateFBXMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, std::vector<FBXVertex>& VertexData);
	float GetHeightAtPosition(Mesh* terrainMesh, float x, float z, const XMFLOAT4X4& worldMatrix);
	bool IsPointInTriangle(XMFLOAT2& pt, XMFLOAT2& v0, XMFLOAT2& v1, XMFLOAT2& v2);
	float ComputeHeightOnTriangle(XMFLOAT3& pt, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2);
};

class FBXUtil {
private:
	FbxManager* Manager{};
	FbxScene* Scene{};
	std::vector<FBXVertex> ParsedVertices{};

public:
	void Init();
	bool LoadFBXFile(const char* filePath);
	bool TriangulateScene();
	void GetVertexData();
	void ProcessNode(FbxNode* node);
	void ProcessNodeForAnimation(FbxNode* node, FbxAnimLayer* animLayer);
	void ProcessAnimation();
	void PrintAnimationStackNames();
	std::vector<FBXVertex> GetVertexVector();
	void ClearVertexVector();
};

extern FBXUtil fbxUtil;