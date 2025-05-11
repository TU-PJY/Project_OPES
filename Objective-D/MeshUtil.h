#pragma once
#include "DirectX_3D.h"
#include <fstream>
#include <unordered_map>
#include <map>
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
	std::string NodeName;
	std::vector<AnimationKeyFrame> KeyFrames;
};

typedef struct {
	float StartTime;
	float EndTime;
} SerializedAnimationInfo;

class Mesh;
typedef struct {
	FbxScene* Scene;
	std::vector<Mesh*> MeshPart;
	std::vector<AnimationChannel> AnimationChannel;
	float StartTime;
	float CurrentTime;
	float TotalTime;

	std::vector<std::string> AnimationStackNames; // 전체 스택 이름들
	std::string CurrentAnimationStackName;        // 현재 선택된 스택 이름
	int CurrentAnimationStackIndex;          // 현재 선택된 스택 인덱스

	// 직렬화 애니메이션 데이터를 가진 경우 해당 컨테이너를 사용한다.
	bool SerilaizedFlag;
	std::map<std::string, SerializedAnimationInfo> SerializedAnimationStacks;
}FBXMesh;

class Mesh {
private:
	int Ref{};

public:
	void AddRef();
	void Release();

	int MeshType{};

	std::unordered_map<int, std::vector<int>> ControlPointToVertexIndices{};

	XMUINT4* BoneIndices{};                      // 정점당 본 인덱스 (최대 4개)
	XMFLOAT4* BoneWeights{};                     // 정점당 본 가중치 (최대 4개)

	std::vector<XMMATRIX> BoneOffsetMatrices{};    // 각 본의 inverse bind pose
	std::vector<int> BoneParentIndices{};          // 각 본의 부모 인덱스
	std::vector<FbxNode*> BoneNodes{};
	FbxNode* FbxNodePtr{};

	std::string NodeName{};

	UINT Vertices{};
	XMFLOAT3* Position{};
	ID3D12Resource* PositionBuffer{};
	ID3D12Resource* PositionUploadBuffer{};
	XMFLOAT3* OriginalPosition{};

	XMFLOAT3* Normal{};
	ID3D12Resource* NormalBuffer{};
	ID3D12Resource* NormalUploadBuffer{};
	XMFLOAT3* OriginalNormal{};

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

	D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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
	void UpdateSkinning(float Time);
};

class FBXUtil {
private:
	FbxManager* Manager{};
	std::vector<FBXVertex> ParsedVertices{};
	FBXMesh* MeshPtr{};

	FbxScene* StaticScene{};

public:
	void Init();
	bool LoadStaticFBXFile(const char* FilePath, Mesh*& TargetMesh);
	bool LoadMultiStaticFBXFile(std::string FilePath, Mesh*& TargetMesh);
	bool LoadAnimatedFBXFile(const char* FilePath, FBXMesh& TargetMesh);
	bool TriangulateAnimatedScene();
	void GetAnimatedVertexData(DeviceSystem& System);
	void ProcessAnimatedNode(FbxNode* Node, DeviceSystem& System);
	bool TriangulateStaticScene();
	bool TriangulateMultiStaticScene();
	void GetSingleStaticVertexData();
	void GetMultiStaticVertexData();
	void ProcessSingleStaticNode(FbxNode* Node);
	void ProcessMultiStaticNode(FbxNode* Node);
	void ProcessNodeForAnimation(FbxNode* Node, FbxAnimLayer* AnimationLayer);
	void ProcessAnimation();
	void PrintAnimationStackNames();
	void ParseSkin(FbxMesh* FMesh, Mesh* MeshPtr);
	void GetBoneMatricesFromScene(Mesh* MeshPtr, float TimeInSeconds, std::vector<XMMATRIX>& OutBoneMatrices);
	void EnumerateAnimationStacks();
	void SelectAnimation(FBXMesh& TargetMesh, const std::string& AnimationName);
	void GetAnimationPlayTime(FBXMesh& TargetMesh, const std::string& AnimationName);
	void ResetCurrentTime(FBXMesh& TargetMesh);
	std::vector<FBXVertex> GetVertexVector();
	void ClearVertexVector();
	void CreateAnimationStacksFromJSON(std::string jsonFile, FBXMesh& TargetMesh);
};

extern FBXUtil fbxUtil;