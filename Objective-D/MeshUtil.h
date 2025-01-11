#pragma once
#include "DirectX_3D.h"
#include <fstream>
#include <unordered_map>

struct MyVertex
{
	float px, py, pz;   // Position
	float nx, ny, nz;   // Normal
	float u, v;         // UV
	int boneIndices[4]; // 영향을 받는 본 인덱스 (최대 4개)
	float boneWeights[4]; // 본 가중치 (최대 4개)
};
extern std::vector<MyVertex> parsedVertices;
extern std::unordered_map<int, std::vector<std::pair<int, float>>> skinningData;

class Mesh {
private:
	int Ref{};

public:
	void AddRef();
	void Release();

protected:
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
	XMUINT4* BoneIndices{};

	ID3D12Resource* BoneWeightBuffer{};
	ID3D12Resource* BoneWeightUploadBuffer{};
	XMFLOAT4* BoneWeights{};

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
	void CreateFBXMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, std::vector<MyVertex>& VertexData);
	float GetHeightAtPosition(Mesh* terrainMesh, float x, float z, const XMFLOAT4X4& worldMatrix);
	bool IsPointInTriangle(XMFLOAT2& pt, XMFLOAT2& v0, XMFLOAT2& v1, XMFLOAT2& v2);
	float ComputeHeightOnTriangle(XMFLOAT3& pt, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2);
};

class FBXUtil {
public:
	FbxManager* manager{};
	FbxScene* scene{};

	void InitializeFBX(FbxManager*& manager, FbxScene*& scene);
	bool LoadFBXFile(FbxManager* manager, FbxScene* scene, const char* filePath);
	void ProcessSkin(FbxMesh* mesh, std::vector<std::vector<int>>& boneIndices, std::vector<std::vector<float>>& boneWeights);
	void PrintVertexData(const std::vector<MyVertex>& VertexVec);
	bool TriangulateScene(FbxManager* pManager, FbxScene* pScene);
	void GetVertexData(FbxScene* scene, std::vector<MyVertex>& VertexVec);
	void ProcessNode(FbxNode* node, std::vector<MyVertex>& VertexVec);
};

extern FBXUtil fbxUtil;