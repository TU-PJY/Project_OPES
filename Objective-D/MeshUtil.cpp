#include "MeshUtil.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

FBXUtil fbxUtil;
std::vector<MyVertex> parsedVertices;
std::unordered_map<int, std::vector<std::pair<int, float>>> skinningData;

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


void FBXUtil::InitializeFBX(FbxManager*& manager, FbxScene*& scene) {
	// FBX Manager 생성
	manager = FbxManager::Create();
	if (!manager) {
		std::cerr << "Error: Unable to create FBX Manager!\n";
		exit(1);
	}
	std::cout << "FBX Manager created.\n";

	// IOSettings 객체 생성 및 초기화
	FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
	manager->SetIOSettings(ios);

	// FBX Scene 생성
	scene = FbxScene::Create(manager, "My Scene");
	if (!scene) {
		std::cerr << "Error: Unable to create FBX Scene!\n";
		exit(1);
	}
	std::cout << "FBX Scene created.\n";
}

bool FBXUtil::LoadFBXFile(FbxManager* manager, FbxScene* scene, const char* filePath) {
	// FBX Importer 생성
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// 파일 열기
	if (!importer->Initialize(filePath, -1, manager->GetIOSettings())) {
		std::cerr << "Error: Unable to initialize importer!\n";
		std::cerr << "Error: " << importer->GetStatus().GetErrorString() << "\n";
		return false;
	}

	// 파일에서 Scene으로 데이터 읽기
	if (!importer->Import(scene)) {
		std::cerr << "Error: Unable to import FBX scene!\n";
		return false;
	}

	std::cout << "FBX file loaded successfully.\n";

	// Importer 삭제
	importer->Destroy();
	return true;
}

bool FBXUtil::TriangulateScene(FbxManager* pManager, FbxScene* pScene) {
	// FbxGeometryConverter 생성
	FbxGeometryConverter geomConverter(pManager);

	// 씬 전체 Triangulate
	// (replace = true 로 설정하면 기존 지오메트리가 삼각화 결과로 대체됩니다.)
	bool result = geomConverter.Triangulate(pScene, /*replace=*/true);

	if (!result)
	{
		std::cerr << "Error: Triangulation failed!\n";
		return false;
	}

	std::cout << "Scene triangulated successfully.\n";
	return true;
}

void FBXUtil::GetVertexData(FbxScene* scene, std::vector<MyVertex>& VertexVec){
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode) {
		for (int i = 0; i < rootNode->GetChildCount(); ++i) {
			ProcessNode(rootNode->GetChild(i), VertexVec);
		}
	}
}

void FBXUtil::ProcessNode(FbxNode* node, std::vector<MyVertex>& VertexVec) {
	std::cout << "Node Name: " << node->GetName() << "\n";

	FbxMesh* mesh = node->GetMesh();
	if (mesh) {
		std::cout << "Processing Mesh: " << node->GetName() << "\n";

		// 컨트롤 포인트(버텍스) 배열
		FbxVector4* controlPoints = mesh->GetControlPoints();
		int controlPointCount = mesh->GetControlPointsCount();

		// 뼈 및 스키닝 데이터 파싱
		std::vector<std::vector<int>> controlPointBoneIndices(controlPointCount);
		std::vector<std::vector<float>> controlPointBoneWeights(controlPointCount);

		ProcessSkin(mesh, controlPointBoneIndices, controlPointBoneWeights);

		// 폴리곤 반복
		int polygonCount = mesh->GetPolygonCount();
		for (int polyIndex = 0; polyIndex < polygonCount; polyIndex++) {
			int vertexCountInPoly = mesh->GetPolygonSize(polyIndex);
			for (int v = 0; v < vertexCountInPoly; v++) {
				int controlPointIndex = mesh->GetPolygonVertex(polyIndex, v);
				if (controlPointIndex < 0) continue;

				// 위치 (Pos)
				FbxVector4 pos = controlPoints[controlPointIndex];
				// 노멀 읽기
				FbxVector4 normal(0, 0, 0, 0);
				bool hasNormal = mesh->GetPolygonVertexNormal(polyIndex, v, normal);
				// UV 읽기
				FbxVector2 uv(0, 0);

				const char* uvSetName = nullptr;
				if (mesh->GetElementUVCount() > 0) {
					FbxLayerElementUV* uvElement = mesh->GetElementUV(0);
					if (uvElement) {
						uvSetName = uvElement->GetName();
					}
				}

				bool unmapped = false;
				bool hasUV = mesh->GetPolygonVertexUV(polyIndex, v, uvSetName, uv, unmapped);

				// Vertex 구조체 생성
				MyVertex vertex{};
				vertex.px = static_cast<float>(pos[0]);
				vertex.py = static_cast<float>(pos[1]);
				vertex.pz = static_cast<float>(pos[2]);

				vertex.nx = hasNormal ? static_cast<float>(normal[0]) : 0.0f;
				vertex.ny = hasNormal ? static_cast<float>(normal[1]) : 0.0f;
				vertex.nz = hasNormal ? static_cast<float>(normal[2]) : 0.0f;

				vertex.u = hasUV ? static_cast<float>(uv[0]) : 0.0f;
				vertex.v = hasUV ? static_cast<float>(uv[1]) : 0.0f;

				// 뼈 데이터 추가
				auto& boneIndices = controlPointBoneIndices[controlPointIndex];
				auto& boneWeights = controlPointBoneWeights[controlPointIndex];

				for (size_t i = 0; i < 4; i++) {
					if (i < boneIndices.size()) {
						vertex.boneIndices[i] = boneIndices[i];
						vertex.boneWeights[i] = boneWeights[i];
					}
					else {
						vertex.boneIndices[i] = 0;
						vertex.boneWeights[i] = 0.0f;
					}
				}

				VertexVec.push_back(vertex);
			}
		}
	}

	// 하위 노드 순회
	for (int i = 0; i < node->GetChildCount(); ++i) {
		ProcessNode(node->GetChild(i), VertexVec);
	}
}

void FBXUtil::ProcessSkin(FbxMesh* mesh, std::vector<std::vector<int>>& boneIndices, std::vector<std::vector<float>>& boneWeights) {
	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	if (skinCount == 0) return;

	FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
	int clusterCount = skin->GetClusterCount();

	for (int clusterIdx = 0; clusterIdx < clusterCount; clusterIdx++) {
		FbxCluster* cluster = skin->GetCluster(clusterIdx);
		if (!cluster) continue;

		int* indices = cluster->GetControlPointIndices();
		double* weights = cluster->GetControlPointWeights();
		int count = cluster->GetControlPointIndicesCount();

		for (int i = 0; i < count; i++) {
			int controlPointIndex = indices[i];
			float weight = static_cast<float>(weights[i]);

			boneIndices[controlPointIndex].push_back(clusterIdx);
			boneWeights[controlPointIndex].push_back(weight);
		}
	}

	// Normalize weights to sum up to 1.0
	for (size_t i = 0; i < boneWeights.size(); i++) {
		float totalWeight = 0.0f;
		for (float weight : boneWeights[i]) {
			totalWeight += weight;
		}
		for (float& weight : boneWeights[i]) {
			weight /= totalWeight;
		}
	}
}

void FBXUtil::PrintVertexData(const std::vector<MyVertex>& VertexVec) {
	std::cout << "\n--- Stored Vertex Data ---\n";
	for (size_t i = 0; i < VertexVec.size(); ++i) {
		const MyVertex& vertex = VertexVec[i];
		std::cout << "Vertex " << i << ": ";
		std::cout << "Pos(" << vertex.px << ", " << vertex.py << ", " << vertex.pz << "), ";
		std::cout << "Normal(" << vertex.nx << ", " << vertex.ny << ", " << vertex.nz << "), ";
		std::cout << "UV(" << vertex.u << ", " << vertex.v << ")\n";
	}
	std::cout << "--- End of Vertex Data ---\n";
}