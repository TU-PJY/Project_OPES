#include "MeshUtil.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Framework.h"

FBXUtil fbxUtil;

// ResourList에서 해당 함수를 사용하여 매쉬를 로드하도록 한다
Mesh::Mesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, char* Directory, int Type) {
	if (Directory) {
		if (Type == MESH_TYPE_TEXT)
			ImportMesh(Device, CmdList, Directory, true);
		else if (Type == MESH_TYPE_BIN)
			ImportMesh(Device, CmdList, Directory, false);
	}
}

Mesh::Mesh() {
	//std::cout << "Mesh object created." << std::endl;
}

Mesh::~Mesh() {
	if (Position) delete[] Position;
	if (Normal) delete[] Normal;
	if (TextureCoords) delete[] TextureCoords;

	if (PnIndices) delete[] PnIndices;

	if (VertexBufferViews) delete[] VertexBufferViews;

	if (PositionBuffer) PositionBuffer->Release();
	if (NormalBuffer) NormalBuffer->Release();
	if (TextureCoordBuffer) TextureCoordBuffer->Release();
	if (IndexBuffer) IndexBuffer->Release();
}

void Mesh::AddRef() {
	Ref++;
}

void Mesh::Release() {
	if (--Ref <= 0)
		delete this;
}

void Mesh::ReleaseUploadBuffers() {
	if (PositionUploadBuffer) PositionUploadBuffer->Release();
	if (NormalUploadBuffer) NormalUploadBuffer->Release();
	if (TextureCoordUploadBuffer) TextureCoordUploadBuffer->Release();
	if (IndexUploadBuffer) IndexUploadBuffer->Release();

	PositionUploadBuffer = NULL;
	NormalUploadBuffer = NULL;
	TextureCoordUploadBuffer = NULL;
	IndexUploadBuffer = NULL;
};

void Mesh::Render(ID3D12GraphicsCommandList* CmdList) {
	CmdList->IASetPrimitiveTopology(PrimitiveTopology);
	CmdList->IASetVertexBuffers(Slot, NumVertexBufferViews, VertexBufferViews);

	if (IndexBuffer) {
		CmdList->IASetIndexBuffer(&IndexBufferView);
		CmdList->DrawIndexedInstanced(Indices, 1, 0, 0, 0);
	}

	else
		CmdList->DrawInstanced(Vertices, 1, Offset, 0);
}

BOOL Mesh::RayIntersectionByTriangle(XMVECTOR& xmRayOrigin, XMVECTOR& xmRayDirection, XMVECTOR v0, XMVECTOR v1, XMVECTOR v2, float* pfNearHitDistance) {
	float fHitDistance;
	BOOL bIntersected = TriangleTests::Intersects(xmRayOrigin, xmRayDirection, v0, v1, v2, fHitDistance);
	if (bIntersected && (fHitDistance < *pfNearHitDistance)) *pfNearHitDistance = fHitDistance;

	return(bIntersected);
}

int Mesh::CheckRayIntersection(XMVECTOR& xmvPickRayOrigin, XMVECTOR& xmvPickRayDirection, float* pfNearHitDistance) {
	int nIntersections = 0;
	bool bIntersected = OOBB.Intersects(xmvPickRayOrigin, xmvPickRayDirection, *pfNearHitDistance);
	if (bIntersected) {
		for (int i = 0; i < Indices; i += 3) {
			XMVECTOR v0 = XMLoadFloat3(&Position[PnIndices[i]]);
			XMVECTOR v1 = XMLoadFloat3(&Position[PnIndices[i + 1]]);
			XMVECTOR v2 = XMLoadFloat3(&Position[PnIndices[i + 2]]);
			BOOL bIntersected = RayIntersectionByTriangle(xmvPickRayOrigin, xmvPickRayDirection, v0, v1, v2, pfNearHitDistance);
			if (bIntersected) nIntersections++;
		}
	}

	return(nIntersections);
}

// 높이 캐시를 비운다.
void Mesh::ClearHeightCache() {
	HeightCache.clear();
	HeightCacheSaved = false;
}

// 높이 캐시에 높이 값을 저장한다.
void Mesh::SetHeightCache(Mesh* terrainMesh, const XMFLOAT4X4& worldMatrix) {
	if (!HeightCacheSaved) {
		XMMATRIX gmtxWorld = XMLoadFloat4x4(&worldMatrix);

		for (UINT i = 0; i < terrainMesh->Indices; ++i) {
			XMFLOAT3 v = terrainMesh->Position[terrainMesh->PnIndices[i]];
			XMVECTOR vWorld = XMVector3Transform(XMLoadFloat3(&v), gmtxWorld);
			XMFLOAT3 worldVertex;
			XMStoreFloat3(&worldVertex, vWorld);
			HeightCache.push_back(worldVertex);
		}

		HeightCacheSaved = true;
	}
}

// 현재 지점의 높이를 구한다.
float Mesh::GetHeightAtPosition(Mesh* terrainMesh, float x, float z, const XMFLOAT4X4& worldMatrix) {
	size_t Size = HeightCache.size();

	for (UINT i = 0; i < Size; i += 3) {
		XMFLOAT3 v0 = HeightCache[i];
		XMFLOAT3 v1 = HeightCache[i + 1];
		XMFLOAT3 v2 = HeightCache[i + 2];

		if (IsPointInTriangle(XMFLOAT2(x, z), XMFLOAT2(v0.x, v0.z), XMFLOAT2(v1.x, v1.z), XMFLOAT2(v2.x, v2.z)))
			return ComputeHeightOnTriangle(XMFLOAT3(x, 0, z), v0, v1, v2);
	}

	return 0.0f;
}

bool Mesh::IsPointInTriangle(XMFLOAT2& pt, XMFLOAT2& v0, XMFLOAT2& v1, XMFLOAT2& v2) {
	float d00 = (v1.x - v0.x) * (v1.x - v0.x) + (v1.y - v0.y) * (v1.y - v0.y);
	float d01 = (v1.x - v0.x) * (v2.x - v0.x) + (v1.y - v0.y) * (v2.y - v0.y);
	float d11 = (v2.x - v0.x) * (v2.x - v0.x) + (v2.y - v0.y) * (v2.y - v0.y);
	float d20 = (pt.x - v0.x) * (v1.x - v0.x) + (pt.y - v0.y) * (v1.y - v0.y);
	float d21 = (pt.x - v0.x) * (v2.x - v0.x) + (pt.y - v0.y) * (v2.y - v0.y);

	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;
	return (u >= 0) && (v >= 0) && (w >= 0);
}

float Mesh::ComputeHeightOnTriangle(XMFLOAT3& pt, XMFLOAT3& v0, XMFLOAT3& v1, XMFLOAT3& v2) {
	XMVECTOR p = XMLoadFloat3(&pt);
	XMVECTOR a = XMLoadFloat3(&v0);
	XMVECTOR b = XMLoadFloat3(&v1);
	XMVECTOR c = XMLoadFloat3(&v2);

	XMVECTOR n = XMVector3Cross(b - a, c - a);
	n = XMVector3Normalize(n);

	float d = -XMVectorGetX(XMVector3Dot(n, a));
	float height = -(XMVectorGetX(XMVector3Dot(n, XMVectorSet(1.0f, 0.0f, 1.0f, 0.0f) * p)) + d) / XMVectorGetY(n);

	return height;
}

void Mesh::UpdateSkinning(float Time) {
	if (!BoneIndices || !BoneWeights) {
		if (FbxNodePtr) {
			FbxTime t;
			t.SetSecondDouble(Time);
			FbxAMatrix Transform = FbxNodePtr->EvaluateGlobalTransform(t);

			XMMATRIX M = XMMATRIX(
				(float)Transform[0][0], (float)Transform[0][1], (float)Transform[0][2], (float)Transform[0][3],
				(float)Transform[1][0], (float)Transform[1][1], (float)Transform[1][2], (float)Transform[1][3],
				(float)Transform[2][0], (float)Transform[2][1], (float)Transform[2][2], (float)Transform[2][3],
				(float)Transform[3][0], (float)Transform[3][1], (float)Transform[3][2], (float)Transform[3][3]
			);

			for (UINT v = 0; v < Vertices; ++v) {
				XMVECTOR p = XMLoadFloat3(&OriginalPosition[v]);
				XMVECTOR n = XMLoadFloat3(&OriginalNormal[v]);

				XMVECTOR tp = XMVector3Transform(p, M);
				XMVECTOR tn = XMVector3TransformNormal(n, M);

				XMStoreFloat3(&Position[v], tp);
				XMStoreFloat3(&Normal[v], tn);
			}
		}
	}

	else {
		std::vector<XMMATRIX> BoneMatrices;
		fbxUtil.GetBoneMatricesFromScene(this, Time, BoneMatrices);

		for (UINT v = 0; v < Vertices; ++v) {
			XMVECTOR SkinnedPosition = XMVectorZero();
			XMVECTOR SkinnedNormal = XMVectorZero();
			XMVECTOR OriginPosition = XMLoadFloat3(&OriginalPosition[v]);
			XMVECTOR OriginNormal = XMLoadFloat3(&OriginalNormal[v]);

			UINT BoneIndex[4] = { BoneIndices[v].x, BoneIndices[v].y, BoneIndices[v].z, BoneIndices[v].w };
			float Weights[4] = { BoneWeights[v].x, BoneWeights[v].y, BoneWeights[v].z, BoneWeights[v].w };

			for (int i = 0; i < 4; ++i) {
				if (Weights[i] > 0.0f && BoneIndex[i] < BoneMatrices.size()) {
					XMVECTOR TransformedPosition = XMVector3Transform(OriginPosition, BoneMatrices[BoneIndex[i]]);
					XMVECTOR TransformedNormal = XMVector3TransformNormal(OriginNormal, BoneMatrices[BoneIndex[i]]);
					SkinnedPosition += TransformedPosition * Weights[i];
					SkinnedNormal += TransformedNormal * Weights[i];
				}
			}
			XMStoreFloat3(&Position[v], SkinnedPosition);
			XMStoreFloat3(&Normal[v], SkinnedNormal);
		}
	}

	void* Mapped = nullptr;
	D3D12_RANGE Read{ 0, 0 };
	PositionBuffer->Map(0, &Read, &Mapped);
	memcpy(Mapped, Position, sizeof(XMFLOAT3) * Vertices);
	PositionBuffer->Unmap(0, nullptr);

	NormalBuffer->Map(0, &Read, &Mapped);
	memcpy(Mapped, Normal, sizeof(XMFLOAT3) * Vertices);
	NormalBuffer->Unmap(0, nullptr);
}


//////////////////////////////////////////////////////////////////////////


void FBXUtil::Init() {
	Manager = FbxManager::Create();
	if (!Manager) {
		std::cerr << "Error: Unable to create FBX Manager!\n";
		exit(1);
	}
	std::cout << "FBX Manager created.\nLoading FBX files...";

	FbxIOSettings* IOS = FbxIOSettings::Create(Manager, IOSROOT);
	Manager->SetIOSettings(IOS);
}

bool FBXUtil::LoadStaticFBXFile(const char* FilePath, Mesh*& TargetMesh) {
	if (!TargetMesh) {
		TargetMesh = new Mesh();
		TargetMesh->MeshType = FBX_STATIC;
	}

	if (StaticScene)
		StaticScene->Destroy();
	StaticScene = FbxScene::Create(Manager, "FBX_Scene");

	if (!StaticScene) {
		std::cerr << "Error: Unable to create FBX Scene!\n";
		exit(1);
	}

	FbxImporter* Importer = FbxImporter::Create(Manager, "");

	if (!Importer->Initialize(FilePath, -1, Manager->GetIOSettings())) {
		std::cerr << "Error: Unable to initialize importer!\n";
		std::cerr << "Error: " << Importer->GetStatus().GetErrorString() << "\n";

		FbxStatus Status = Importer->GetStatus();
		std::cerr << "Detailed Status Code: " << Status.GetCode() << "\n";
		return false;
	}

	if (!Importer->Import(StaticScene)) {
		std::cerr << "Error: Unable to import FBX scene!\n";
		return false;
	}

	FbxAxisSystem directX(FbxAxisSystem::eDirectX);
	directX.DeepConvertScene(StaticScene);
	FbxSystemUnit::m.ConvertScene(StaticScene);

	//std::cout << "FBX file loaded successfully.\n";
	Importer->Destroy();

	return true;
}

bool FBXUtil::TriangulateStaticScene() {
	FbxGeometryConverter GeometryConverter(Manager);
	bool Result = GeometryConverter.Triangulate(StaticScene, true);

	if (!Result) {
		std::cerr << "Error: Triangulation failed!\n";
		return false;
	}

	//std::cout << "Scene triangulated successfully.\n";
	return true;
}

void FBXUtil::GetStaticVertexData() {
	FbxNode* RootNode = StaticScene->GetRootNode();

	if (RootNode) {
		for (int i = 0; i < RootNode->GetChildCount(); ++i)
			ProcessStaticNode(RootNode->GetChild(i));
	}
}

void FBXUtil::ProcessStaticNode(FbxNode* Node) {
	//std::cout << "Node Name: " << Node->GetName() << "\n";

	FbxMesh* FMesh = Node->GetMesh();
	if (FMesh) {
		FbxVector4* ControlPoints = FMesh->GetControlPoints();
		int ControlPointCount = FMesh->GetControlPointsCount();

		int PolygonCount = FMesh->GetPolygonCount();
		for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++) {
			int VertexCountInPolygon = FMesh->GetPolygonSize(PolyIndex);

			for (int V = 0; V < VertexCountInPolygon; V++) {
				int ControlPointIndex = FMesh->GetPolygonVertex(PolyIndex, V);
				if (ControlPointIndex < 0)
					continue;

				FbxVector4 Position = ControlPoints[ControlPointIndex];
				FbxVector4 Normal(0, 0, 0, 0);

				bool HasNormal = FMesh->GetPolygonVertexNormal(PolyIndex, V, Normal);
				FbxVector2 UV(0, 0);

				const char* UVSetName = nullptr;
				if (FMesh->GetElementUVCount() > 0) {
					FbxLayerElementUV* UVElement = FMesh->GetElementUV(0);
					if (UVElement)
						UVSetName = UVElement->GetName();
				}

				bool Unmapped = false;
				bool HasUV = FMesh->GetPolygonVertexUV(PolyIndex, V, UVSetName, UV, Unmapped);

				FBXVertex Vertex{};
				Vertex.px = static_cast<float>(Position[0]);
				Vertex.py = static_cast<float>(Position[1]);
				Vertex.pz = static_cast<float>(Position[2]);

				Vertex.nx = HasNormal ? static_cast<float>(Normal[0]) : 0.0f;
				Vertex.ny = HasNormal ? static_cast<float>(Normal[1]) : 0.0f;
				Vertex.nz = HasNormal ? static_cast<float>(Normal[2]) : 0.0f;

				Vertex.u = HasUV ? static_cast<float>(UV[0]) : 0.0f;
				Vertex.v = HasUV ? static_cast<float>(UV[1]) : 0.0f;

				ParsedVertices.push_back(Vertex);
			}
		}
	}

	for (int i = 0; i < Node->GetChildCount(); ++i)
		ProcessStaticNode(Node->GetChild(i));
}

bool FBXUtil::LoadAnimatedFBXFile(const char* FilePath, FBXMesh& TargetMesh) {
	FbxScene* Scene = FbxScene::Create(Manager, "FBX_Scene");
	if (!Scene) {
		std::cerr << "Error: Unable to create FBX Scene!\n";
		exit(1);
	}
	MeshPtr = &TargetMesh;
	TargetMesh.Scene = Scene;

	FbxImporter* Importer = FbxImporter::Create(Manager, "");

	if (!Importer->Initialize(FilePath, -1, Manager->GetIOSettings())) {
		std::cerr << "Error: Unable to initialize importer!\n";
		std::cerr << "Error: " << Importer->GetStatus().GetErrorString() << "\n";

		FbxStatus Status = Importer->GetStatus();
		std::cerr << "Detailed Status Code: " << Status.GetCode() << "\n";
		return false;
	}

	if (!Importer->Import(Scene)) {
		std::cerr << "Error: Unable to import FBX scene!\n";
		return false;
	}

	FbxAxisSystem directX(FbxAxisSystem::eDirectX);
	directX.DeepConvertScene(Scene);
	FbxSystemUnit::m.ConvertScene(Scene);

	//std::cout << "FBX file loaded successfully.\n";
	Importer->Destroy();

	return true;
}

bool FBXUtil::TriangulateAnimatedScene() {
	FbxGeometryConverter GeometryConverter(Manager);
	bool Result = GeometryConverter.Triangulate(MeshPtr->Scene, true);

	if (!Result) {
		std::cerr << "Error: Triangulation failed!\n";
		return false;
	}

	//std::cout << "Scene triangulated successfully.\n";
	return true;
}

void FBXUtil::GetAnimatedVertexData(DeviceSystem& System) {
	FbxNode* RootNode = MeshPtr->Scene->GetRootNode();

	if (RootNode) {
		for (int i = 0; i < RootNode->GetChildCount(); ++i)
			ProcessAnimatedNode(RootNode->GetChild(i), System);
	}
}

void FBXUtil::ProcessAnimatedNode(FbxNode* Node, DeviceSystem& System) {
	//std::cout << "Node Name: " << Node->GetName() << "\n";

	FbxMesh* FMesh = Node->GetMesh();
	if (FMesh) {
		ParsedVertices.clear();

		FbxVector4* ControlPoints = FMesh->GetControlPoints();
		int PolygonCount = FMesh->GetPolygonCount();
		int NumControlPoint{};
		int VertexIndex{};

		Mesh* NewMesh = new Mesh();
		NewMesh->MeshType = FBX_ANIMATED;
		NewMesh->FbxNodePtr = Node;

		for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++) {
			int VertexCountInPolygon = FMesh->GetPolygonSize(PolyIndex);

			for (int V = 0; V < VertexCountInPolygon; V++) {
				int ControlPointIndex = FMesh->GetPolygonVertex(PolyIndex, V);
				int GlobalVertexIndex = ParsedVertices.size();
				NumControlPoint = ControlPointIndex;
				VertexIndex = GlobalVertexIndex;

				if (ControlPointIndex < 0) continue;

				FbxVector4 Position = ControlPoints[ControlPointIndex];
				FbxVector4 Normal(0, 0, 0, 0);
				bool HasNormal = FMesh->GetPolygonVertexNormal(PolyIndex, V, Normal);

				FbxVector2 UV(0, 0);
				const char* UVSetName = nullptr;
				if (FMesh->GetElementUVCount() > 0) {
					FbxLayerElementUV* UVElement = FMesh->GetElementUV(0);
					if (UVElement) UVSetName = UVElement->GetName();
				}
				bool Unmapped = false;
				bool HasUV = FMesh->GetPolygonVertexUV(PolyIndex, V, UVSetName, UV, Unmapped);

				FBXVertex Vertex{};
				Vertex.px = static_cast<float>(Position[0]);
				Vertex.py = static_cast<float>(Position[1]);
				Vertex.pz = static_cast<float>(Position[2]);
				Vertex.nx = HasNormal ? static_cast<float>(Normal[0]) : 0.0f;
				Vertex.ny = HasNormal ? static_cast<float>(Normal[1]) : 0.0f;
				Vertex.nz = HasNormal ? static_cast<float>(Normal[2]) : 0.0f;
				Vertex.u = HasUV ? static_cast<float>(UV[0]) : 0.0f;
				Vertex.v = HasUV ? static_cast<float>(UV[1]) : 0.0f;

				ParsedVertices.push_back(Vertex);
				int vertexIndex = ParsedVertices.size() - 1;
				NewMesh->ControlPointToVertexIndices[NumControlPoint].push_back(vertexIndex);
			}
		}
		NewMesh->NodeName = Node->GetName();
		NewMesh->CreateFBXMesh(System.Device, System.CmdList, ParsedVertices);
		fbxUtil.ParseSkin(FMesh, NewMesh);
		MeshPtr->MeshPart.push_back(NewMesh);
	}

	for (int i = 0; i < Node->GetChildCount(); ++i)
		ProcessAnimatedNode(Node->GetChild(i), System);
}

void FBXUtil::ProcessNodeForAnimation(FbxNode* Node, FbxAnimLayer* AnimationLayer) {
	AnimationChannel Channel;
	Channel.NodeName = Node->GetName();

	FbxAnimCurve* TransformX = Node->LclTranslation.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* TransformY = Node->LclTranslation.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* TransformZ = Node->LclTranslation.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	FbxAnimCurve* RotationX = Node->LclRotation.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* RotationY = Node->LclRotation.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* RotationZ = Node->LclRotation.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	FbxAnimCurve* ScaleX = Node->LclScaling.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* ScaleY = Node->LclScaling.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* ScaleZ = Node->LclScaling.GetCurve(AnimationLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	std::set<FbxTime> KeyTimes;

	auto CollectTimes = 
		[&KeyTimes](FbxAnimCurve* Curve) {
		if (!Curve) 
			return;
		int Counts = Curve->KeyGetCount();
		for (int i = 0; i < Counts; ++i)
			KeyTimes.insert(Curve->KeyGetTime(i));
		};

	CollectTimes(TransformX); 
	CollectTimes(TransformY); 
	CollectTimes(TransformZ);

	CollectTimes(RotationX);   
	CollectTimes(RotationY);   
	CollectTimes(RotationZ);

	CollectTimes(ScaleX); 
	CollectTimes(ScaleY); 
	CollectTimes(ScaleZ);

	for (const FbxTime& Time : KeyTimes) {
		AnimationKeyFrame Key{};
		Key.time = static_cast<float>(Time.GetSecondDouble());

		Key.translation[0] = TransformX ? static_cast<float>(TransformX->Evaluate(Time)) : 0.0f;
		Key.translation[1] = TransformY ? static_cast<float>(TransformY->Evaluate(Time)) : 0.0f;
		Key.translation[2] = TransformZ ? static_cast<float>(TransformZ->Evaluate(Time)) : 0.0f;

		Key.rotation[0] = RotationX ? static_cast<float>(RotationX->Evaluate(Time)) : 0.0f;
		Key.rotation[1] = RotationY ? static_cast<float>(RotationY->Evaluate(Time)) : 0.0f;
		Key.rotation[2] = RotationZ ? static_cast<float>(RotationZ->Evaluate(Time)) : 0.0f;

		Key.scale[0] = ScaleX ? static_cast<float>(ScaleX->Evaluate(Time)) : 1.0f;
		Key.scale[1] = ScaleY ? static_cast<float>(ScaleY->Evaluate(Time)) : 1.0f;
		Key.scale[2] = ScaleZ ? static_cast<float>(ScaleZ->Evaluate(Time)) : 1.0f;

		Channel.KeyFrames.push_back(Key);
	}

	if (!Channel.KeyFrames.empty()) 
		MeshPtr->AnimationChannel.push_back(Channel);
	
	for (int i = 0; i < Node->GetChildCount(); ++i) 
		ProcessNodeForAnimation(Node->GetChild(i), AnimationLayer);
}

void FBXUtil::ProcessAnimation() {
	int AnimationStackCount = MeshPtr->Scene->GetSrcObjectCount<FbxAnimStack>();
	if (AnimationStackCount > 0) {
		FbxAnimStack* AnimationStack = MeshPtr->Scene->GetSrcObject<FbxAnimStack>(0);
		MeshPtr->Scene->SetCurrentAnimationStack(AnimationStack);
		FbxAnimLayer* AnimationLayer = AnimationStack->GetMember<FbxAnimLayer>(0);

		ProcessNodeForAnimation(MeshPtr->Scene->GetRootNode(), AnimationLayer);
	}
}

void FBXUtil::PrintAnimationStackNames() {
	int AnimationStackCount = MeshPtr->Scene->GetSrcObjectCount<FbxAnimStack>();
	std::cout << "Total animation stacks: " << AnimationStackCount << "\n";
	for (int i = 0; i < AnimationStackCount; ++i) {
		FbxAnimStack* AnimationStack = MeshPtr->Scene->GetSrcObject<FbxAnimStack>(i);
		if (AnimationStack)
			std::cout << "Animation stack [" << i << "]: " << AnimationStack->GetName() << "\n";
	}
}

void FBXUtil::ParseSkin(FbxMesh* FMesh, Mesh* MeshPtr) {
	std::unordered_map<int, std::vector<std::pair<int, float>>> VertexSkinData;

	auto* Skin = static_cast<FbxSkin*>(FMesh->GetDeformer(0, FbxDeformer::eSkin));
	if (!Skin) return;

	int ClusterCount = Skin->GetClusterCount();
	MeshPtr->BoneOffsetMatrices.resize(ClusterCount);
	MeshPtr->BoneParentIndices.resize(ClusterCount, -1);
	MeshPtr->BoneNodes.resize(ClusterCount);

	MeshPtr->BoneIndices = new XMUINT4[MeshPtr->Vertices]{};
	MeshPtr->BoneWeights = new XMFLOAT4[MeshPtr->Vertices]{};

	for (int c = 0; c < ClusterCount; ++c) {
		FbxCluster* Cluster = Skin->GetCluster(c);
		FbxNode* BoneNode = Cluster->GetLink();
		MeshPtr->BoneNodes[c] = BoneNode;

		FbxAMatrix MeshBindGlobal;
		FbxAMatrix BoneBindGlobal;

		Cluster->GetTransformMatrix(MeshBindGlobal);
		Cluster->GetTransformLinkMatrix(BoneBindGlobal);

		FbxAMatrix OffsetMatrix = BoneBindGlobal.Inverse() * MeshBindGlobal;
		XMMATRIX XMOffset = XMMATRIX(
			(float)OffsetMatrix[0][0], (float)OffsetMatrix[0][1], (float)OffsetMatrix[0][2], (float)OffsetMatrix[0][3],
			(float)OffsetMatrix[1][0], (float)OffsetMatrix[1][1], (float)OffsetMatrix[1][2], (float)OffsetMatrix[1][3],
			(float)OffsetMatrix[2][0], (float)OffsetMatrix[2][1], (float)OffsetMatrix[2][2], (float)OffsetMatrix[2][3],
			(float)OffsetMatrix[3][0], (float)OffsetMatrix[3][1], (float)OffsetMatrix[3][2], (float)OffsetMatrix[3][3]
		);
		MeshPtr->BoneOffsetMatrices[c] = XMOffset;
		
		FbxNode* Parent = BoneNode->GetParent();
		for (int p = 0; p < ClusterCount; ++p) {
			if (Skin->GetCluster(p)->GetLink() == Parent) {
				MeshPtr->BoneParentIndices[c] = p;
				break;
			}
		}

		int* Indices = Cluster->GetControlPointIndices();
		double* Weights = Cluster->GetControlPointWeights();
		int Count = Cluster->GetControlPointIndicesCount();

		for (int i = 0; i < Count; ++i) {
			int ControlIndex = Indices[i];
			float Weight = static_cast<float>(Weights[i]);

			auto Found = MeshPtr->ControlPointToVertexIndices.find(ControlIndex);
			if (Found == MeshPtr->ControlPointToVertexIndices.end()) 
				continue;

			for (int VertexIndex : Found->second)
				VertexSkinData[VertexIndex].emplace_back(c, Weight);
		}

		for (auto& [VertexIndex, SkinList] : VertexSkinData) {
			std::sort(SkinList.begin(), SkinList.end(), [](auto& a, auto& b) {
				return a.second > b.second;
				});

			XMUINT4& BoneIndice = MeshPtr->BoneIndices[VertexIndex];
			XMFLOAT4& BoneWeight = MeshPtr->BoneWeights[VertexIndex];

			float TotalWeight = 0.f;
			for (int i = 0; i < 4 && i < SkinList.size(); ++i) {
				(&BoneIndice.x)[i] = SkinList[i].first;
				(&BoneWeight.x)[i] = SkinList[i].second;
				TotalWeight += SkinList[i].second;
			}

			if (TotalWeight > 0.f) {
				BoneWeight.x /= TotalWeight;
				BoneWeight.y /= TotalWeight;
				BoneWeight.z /= TotalWeight;
				BoneWeight.w /= TotalWeight;
			}
		}
	}

	for (UINT v = 0; v < MeshPtr->Vertices; ++v) {
		XMFLOAT4& bw = MeshPtr->BoneWeights[v];
		if (bw.x + bw.y + bw.z + bw.w == 0.0f) {
			MeshPtr->BoneIndices[v] = XMUINT4(0, 0, 0, 0);
			MeshPtr->BoneWeights[v] = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}
}

void FBXUtil::GetBoneMatricesFromScene(Mesh* MeshPtr, float TimeInSeconds, std::vector<XMMATRIX>& OutBoneMatrices) {
	UINT B = static_cast<UINT>(MeshPtr->BoneNodes.size());

	OutBoneMatrices.resize(B);
	std::vector<XMMATRIX> Global(B);

	FbxTime Time;
	Time.SetSecondDouble(TimeInSeconds);

	for (UINT i = 0; i < B; ++i) {
		FbxNode* BoneNode = MeshPtr->BoneNodes[i];

		if (!BoneNode) {
			Global[i] = XMMatrixIdentity();
			continue;
		}

		FbxAMatrix Matrix = BoneNode->EvaluateGlobalTransform(Time);
		XMMATRIX XM = XMMATRIX(
			(float)Matrix[0][0], (float)Matrix[0][1], (float)Matrix[0][2], (float)Matrix[0][3],
			(float)Matrix[1][0], (float)Matrix[1][1], (float)Matrix[1][2], (float)Matrix[1][3],
			(float)Matrix[2][0], (float)Matrix[2][1], (float)Matrix[2][2], (float)Matrix[2][3],
			(float)Matrix[3][0], (float)Matrix[3][1], (float)Matrix[3][2], (float)Matrix[3][3]
		);

		Global[i] = XM;
	}

	for (UINT i = 0; i < B; ++i)
		OutBoneMatrices[i] = MeshPtr->BoneOffsetMatrices[i] * Global[i];
}

void FBXUtil::EnumerateAnimationStacks() {
	MeshPtr->AnimationStackNames.clear();
	int Count = MeshPtr->Scene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < Count; ++i) {
		FbxAnimStack* Stack = MeshPtr->Scene->GetSrcObject<FbxAnimStack>(i);
		if (Stack) {
			std::string StackName = Stack->GetName();
			MeshPtr->AnimationStackNames.push_back(StackName);
		}
	}

	if (!MeshPtr->AnimationStackNames.empty()) {
		MeshPtr->CurrentAnimationStackName = MeshPtr->AnimationStackNames[0];
		MeshPtr->CurrentAnimationStackIndex = 0;
		SelectAnimation(*MeshPtr, MeshPtr->AnimationStackNames[0]);
	}
}

void FBXUtil::SelectAnimation(FBXMesh& TargetMesh, const std::string& AnimationName) {
	int Count = TargetMesh.Scene->GetSrcObjectCount<FbxAnimStack>();

	for (int i = 0; i < Count; ++i) {
		FbxAnimStack* Stack = TargetMesh.Scene->GetSrcObject<FbxAnimStack>(i);

		if (Stack && AnimationName == Stack->GetName()) {
			TargetMesh.Scene->SetCurrentAnimationStack(Stack);
			TargetMesh.CurrentAnimationStackName = AnimationName;
			TargetMesh.CurrentAnimationStackIndex = i;
			GetAnimationPlayTime(TargetMesh, AnimationName);
		}
	}
}

void FBXUtil::GetAnimationPlayTime(FBXMesh& TargetMesh, const std::string& AnimationName) {
	FbxAnimStack* stack = TargetMesh.Scene->FindMember<FbxAnimStack>(AnimationName.c_str());
	if (!stack)
		TargetMesh.TotalTime = 0.0;

	FbxTimeSpan span = stack->GetLocalTimeSpan();
	FbxTime start = span.GetStart();
	FbxTime end = span.GetStop();

	TargetMesh.TotalTime = (end - start).GetSecondDouble();
}

void FBXUtil::ResetCurrentTime(FBXMesh& TargetMesh) {
	TargetMesh.CurrentTime = 0.0;
}

std::vector<FBXVertex> FBXUtil::GetVertexVector() {
	return ParsedVertices;
}

void FBXUtil::ClearVertexVector() {
	ParsedVertices.clear();
}