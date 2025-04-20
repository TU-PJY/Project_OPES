#include "MeshUtil.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Framework.h"

FBXUtil fbxUtil;
std::vector<AnimationChannel> AnimationChannels;
std::vector<Mesh*> AnimatedMesh;

// 매쉬를 담당하는 유틸이다.

// ResourList에서 해당 함수를 사용하여 매쉬를 로드하도록 한다
Mesh::Mesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, char* Directory, int Type) {
	if (Directory) {
		if (Type == MESH_TYPE_TEXT)
			ImportMesh(Device, CmdList, Directory, true);
		else if (Type == MESH_TYPE_BIN)
			ImportMesh(Device, CmdList, Directory, false);
	}
}

Mesh::Mesh() {}

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
	/*if (PositionUploadBuffer) PositionUploadBuffer->Release();
	if (NormalUploadBuffer) NormalUploadBuffer->Release();
	if (TextureCoordUploadBuffer) TextureCoordUploadBuffer->Release();
	if (IndexUploadBuffer) IndexUploadBuffer->Release();

	PositionUploadBuffer = NULL;
	NormalUploadBuffer = NULL;
	TextureCoordUploadBuffer = NULL;
	IndexUploadBuffer = NULL;*/
};

void Mesh::Render(ID3D12GraphicsCommandList* CmdList) {
	CmdList->IASetPrimitiveTopology(PromitiveTopology);
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


//////////////////////////////////////////////////////////////////////////


void FBXUtil::Init() {
	Manager = FbxManager::Create();
	if (!Manager) {
		std::cerr << "Error: Unable to create FBX Manager!\n";
		exit(1);
	}
	std::cout << "FBX Manager created.\n";

	FbxIOSettings* IOS = FbxIOSettings::Create(Manager, IOSROOT);
	Manager->SetIOSettings(IOS);
}

bool FBXUtil::LoadFBXFile(const char* filePath) {
	if (Scene) {
		Scene->Destroy();
		Scene = nullptr;
		Scene = FbxScene::Create(Manager, "FBX_Scene");
		if (!Scene) {
			std::cerr << "Error: Unable to create FBX Scene!\n";
			exit(1);
		}
	}

	else {
		Scene = FbxScene::Create(Manager, "FBX_Scene");
		if (!Scene) {
			std::cerr << "Error: Unable to create FBX Scene!\n";
			exit(1);
		}
	}
	
	FbxImporter* Importer = FbxImporter::Create(Manager, "");

	if (!Importer->Initialize(filePath, -1, Manager->GetIOSettings())) {
		std::cerr << "Error: Unable to initialize importer!\n";
		std::cerr << "Error: " << Importer->GetStatus().GetErrorString() << "\n";
		FbxStatus status = Importer->GetStatus();
		std::cerr << "Detailed Status Code: " << status.GetCode() << "\n";
		return false;
	}

	if (!Importer->Import(Scene)) {
		std::cerr << "Error: Unable to import FBX scene!\n";
		return false;
	}
	std::cout << "FBX file loaded successfully.\n";

	Importer->Destroy();

	return true;
}

bool FBXUtil::TriangulateScene() {
	FbxGeometryConverter GeometryConverter(Manager);
	bool Result = GeometryConverter.Triangulate(Scene, true);

	if (!Result) {
		std::cerr << "Error: Triangulation failed!\n";
		return false;
	}

	std::cout << "Scene triangulated successfully.\n";
	return true;
}

void FBXUtil::GetVertexData(){
	FbxNode* RootNode = Scene->GetRootNode();

	if (RootNode) {
		for (int i = 0; i < RootNode->GetChildCount(); ++i) 
			ProcessNode(RootNode->GetChild(i));
	}
}

void FBXUtil::ProcessNode(FbxNode* Node) {
	std::cout << "Node Name: " << Node->GetName() << "\n";

	FbxMesh* fbxMesh = Node->GetMesh();
	if (fbxMesh) {
		std::cout << "Processing Mesh: " << Node->GetName() << "\n";

		ParsedVertices.clear(); // 이 노드에 대해 정점 버퍼 초기화

		FbxVector4* ControlPoints = fbxMesh->GetControlPoints();
		int PolygonCount = fbxMesh->GetPolygonCount();

		for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++) {
			int VertexCountInPolygon = fbxMesh->GetPolygonSize(PolyIndex);

			for (int V = 0; V < VertexCountInPolygon; V++) {
				int ControlPointIndex = fbxMesh->GetPolygonVertex(PolyIndex, V);
				if (ControlPointIndex < 0) continue;

				FbxVector4 Position = ControlPoints[ControlPointIndex];
				FbxVector4 Normal(0, 0, 0, 0);
				bool HasNormal = fbxMesh->GetPolygonVertexNormal(PolyIndex, V, Normal);

				FbxVector2 UV(0, 0);
				const char* UVSetName = nullptr;
				if (fbxMesh->GetElementUVCount() > 0) {
					FbxLayerElementUV* UVElement = fbxMesh->GetElementUV(0);
					if (UVElement) UVSetName = UVElement->GetName();
				}
				bool Unmapped = false;
				bool HasUV = fbxMesh->GetPolygonVertexUV(PolyIndex, V, UVSetName, UV, Unmapped);

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

		// 새로운 Mesh 객체 생성
		Mesh* NewMesh = new Mesh();
		NewMesh->nodeName = Node->GetName(); // Mesh 이름 저장 (선택사항)
		NewMesh->CreateFBXMesh(framework.Device, framework.CmdList, ParsedVertices); // 장치 및 커맨드리스트 필요

		// AnimatedMesh 리스트에 저장
		AnimatedMesh.push_back(NewMesh);
	}

	// 자식 노드 재귀 처리
	for (int i = 0; i < Node->GetChildCount(); ++i)
		ProcessNode(Node->GetChild(i));
}

void FBXUtil::ProcessNodeForAnimation(FbxNode* node, FbxAnimLayer* animLayer) {
	AnimationChannel channel;
	channel.nodeName = node->GetName();

	// Translation curves
	FbxAnimCurve* transX = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* transY = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* transZ = node->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	// Rotation curves
	FbxAnimCurve* rotX = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* rotY = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* rotZ = node->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	// Scaling curves
	FbxAnimCurve* scaleX = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* scaleY = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* scaleZ = node->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	// 통합 시간 수집 (Set으로 중복 제거 및 정렬)
	std::set<FbxTime> keyTimes;

	auto CollectTimes = [&keyTimes](FbxAnimCurve* curve) {
		if (!curve) return;
		int count = curve->KeyGetCount();
		for (int i = 0; i < count; ++i)
			keyTimes.insert(curve->KeyGetTime(i));
		};

	CollectTimes(transX); CollectTimes(transY); CollectTimes(transZ);
	CollectTimes(rotX);   CollectTimes(rotY);   CollectTimes(rotZ);
	CollectTimes(scaleX); CollectTimes(scaleY); CollectTimes(scaleZ);

	// 시간 기준으로 키프레임 생성
	for (const FbxTime& time : keyTimes) {
		AnimationKeyFrame key{};
		key.time = static_cast<float>(time.GetSecondDouble());

		// Translation
		key.translation[0] = transX ? static_cast<float>(transX->Evaluate(time)) : 0.0f;
		key.translation[1] = transY ? static_cast<float>(transY->Evaluate(time)) : 0.0f;
		key.translation[2] = transZ ? static_cast<float>(transZ->Evaluate(time)) : 0.0f;

		// Rotation
		key.rotation[0] = rotX ? static_cast<float>(rotX->Evaluate(time)) : 0.0f;
		key.rotation[1] = rotY ? static_cast<float>(rotY->Evaluate(time)) : 0.0f;
		key.rotation[2] = rotZ ? static_cast<float>(rotZ->Evaluate(time)) : 0.0f;

		// Scale
		key.scale[0] = scaleX ? static_cast<float>(scaleX->Evaluate(time)) : 1.0f;
		key.scale[1] = scaleY ? static_cast<float>(scaleY->Evaluate(time)) : 1.0f;
		key.scale[2] = scaleZ ? static_cast<float>(scaleZ->Evaluate(time)) : 1.0f;

		channel.keyframes.push_back(key);
	}

	// 키프레임이 존재하면 채널 저장
	if (!channel.keyframes.empty()) {
		AnimationChannels.push_back(channel);
	}

	// 자식 노드 재귀 처리
	for (int i = 0; i < node->GetChildCount(); ++i) {
		ProcessNodeForAnimation(node->GetChild(i), animLayer);
	}
}

void FBXUtil::ProcessAnimation() {
	int animStackCount = Scene->GetSrcObjectCount<FbxAnimStack>();
	if (animStackCount > 0) {
		// 첫 번째 애니메이션 스택 선택 (필요에 따라 다른 스택 선택 가능)
		FbxAnimStack* animStack = Scene->GetSrcObject<FbxAnimStack>(0);
		Scene->SetCurrentAnimationStack(animStack);
		FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(0);

		// 루트 노드부터 애니메이션 데이터 처리 시작
		ProcessNodeForAnimation(Scene->GetRootNode(), animLayer);
	}
}

void FBXUtil::PrintAnimationStackNames() {
	int animStackCount = Scene->GetSrcObjectCount<FbxAnimStack>();
	std::cout << "Total animation stacks: " << animStackCount << "\n";
	for (int i = 0; i < animStackCount; ++i) {
		FbxAnimStack* animStack = Scene->GetSrcObject<FbxAnimStack>(i);
		if (animStack) {
			std::cout << "Animation stack [" << i << "]: " << animStack->GetName() << "\n";
		}
	}
}

std::vector<FBXVertex> FBXUtil::GetVertexVector() {
	return ParsedVertices;
}

void FBXUtil::ClearVertexVector(){
	ParsedVertices.clear();
}