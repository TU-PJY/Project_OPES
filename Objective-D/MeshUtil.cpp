#include "MeshUtil.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Framework.h"
#include "Config.h"

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

void Mesh::Render(ID3D12GraphicsCommandList* CmdList, D3D12_VERTEX_BUFFER_VIEW*& VBuff) {
	CmdList->IASetPrimitiveTopology(PrimitiveTopology);
	CmdList->IASetVertexBuffers(Slot, 3, VBuff);

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

void Mesh::UpdateSkinning(FBXMesh& Source, std::vector<XMMATRIX>& BoneMatrices, void*& PMap, void*& NMap, float Time) {
	if (!BoneIndices || !BoneWeights)
		return;

	for (UINT v = 0; v < Vertices; ++v) {
		XMVECTOR SkinnedPosition = XMVectorZero();
		XMVECTOR SkinnedNormal = XMVectorZero();
		XMVECTOR PositionOrigin = XMLoadFloat3(&OriginalPosition[v]);
		XMVECTOR NormalOrigin = XMLoadFloat3(&OriginalNormal[v]);

		UINT BoneIndex[4] = { BoneIndices[v].x, BoneIndices[v].y, BoneIndices[v].z, BoneIndices[v].w };
		float BoneWeight[4] = { BoneWeights[v].x, BoneWeights[v].y, BoneWeights[v].z, BoneWeights[v].w };

		for (int i = 0; i < 4; ++i) {
			if (BoneWeight[i] > 0.0f && BoneIndex[i] < BoneMatrices.size()) {
				XMMATRIX transform = BoneMatrices[BoneIndex[i]];
				SkinnedPosition += XMVector3Transform(PositionOrigin, transform) * BoneWeight[i];
				SkinnedNormal += XMVector3TransformNormal(NormalOrigin, transform) * BoneWeight[i];
			}
		}

		XMStoreFloat3(&Position[v], SkinnedPosition);
		XMStoreFloat3(&Normal[v], SkinnedNormal);
	}

	memcpy(PMap, Position, sizeof(XMFLOAT3) * Vertices);
	memcpy(NMap, Normal, sizeof(XMFLOAT3) * Vertices);
}

void Mesh::UpdateSkinning(FBXMesh& Source, float Time) {
	if (!BoneIndices || !BoneWeights)
		return;

	std::string SearchName;
	if (!Source.SerializedFlag)
		SearchName = Source.CurrentAnimationStackName;
	else
		SearchName = Source.AnimationStackNames[0];

	auto FoundFrames = PrecomputedBoneMatrices.find(SearchName);
	if (FoundFrames == PrecomputedBoneMatrices.end()) 
		return;

	int FrameIndex = std::clamp(static_cast<int>(Time * AnimationExtractFrame), 0, (int)FoundFrames->second.size() - 1);
	const std::vector<XMMATRIX>& BoneMatrices = FoundFrames->second[FrameIndex];

	for (UINT v = 0; v < Vertices; ++v) {
		XMVECTOR SkinnedPosition = XMVectorZero();
		XMVECTOR SkinnedNormal = XMVectorZero();
		XMVECTOR PositionOrigin = XMLoadFloat3(&OriginalPosition[v]);
		XMVECTOR NormalOrigin = XMLoadFloat3(&OriginalNormal[v]);

		UINT BoneIndex[4] = { BoneIndices[v].x, BoneIndices[v].y, BoneIndices[v].z, BoneIndices[v].w };
		float BoneWeight[4] = { BoneWeights[v].x, BoneWeights[v].y, BoneWeights[v].z, BoneWeights[v].w };

		for (int i = 0; i < 4; ++i) {
			if (BoneWeight[i] > 0.0f && BoneIndex[i] < BoneMatrices.size()) {
				XMMATRIX transform = BoneMatrices[BoneIndex[i]];
				SkinnedPosition += XMVector3Transform(PositionOrigin, transform) * BoneWeight[i];
				SkinnedNormal += XMVector3TransformNormal(NormalOrigin, transform) * BoneWeight[i];
			}
		}

		XMStoreFloat3(&Position[v], SkinnedPosition);
		XMStoreFloat3(&Normal[v], SkinnedNormal);
	}

	memcpy(PositionMapped, Position, sizeof(XMFLOAT3) * Vertices);
	memcpy(NormalMapped, Normal, sizeof(XMFLOAT3) * Vertices);
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
}

void Mesh::UpdateSkinning(void*& PMap, void*& NMap, float Time) {
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

	memcpy(PMap, Position, sizeof(XMFLOAT3) * Vertices);
	memcpy(NMap, Normal, sizeof(XMFLOAT3) * Vertices);
}