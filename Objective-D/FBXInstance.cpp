#include "MeshUtil.h"
#include "Config.h"

FBX::FBX(FBXMesh& TargetFBX, bool StopState) {
	if (InitState)
		return;

	FBXPtr = &TargetFBX;
	Serialized = FBXPtr->SerializedFlag;
	SelectAnimation(FBXPtr->CurrentAnimationStackName);
	CreateBuffer(GlobalSystem);

	if (StopState)
		StopAnimationUpdate();

	DestDelay = 1.0 / (float)AnimationExtractFrame;
}

FBX::FBX() {}

FBX::~FBX() {
	ReleaseBuffer();
}

void FBX::SelectFBXMesh(FBXMesh& TargetFBX, bool StopState) {
	if (InitState)
		return;

	FBXPtr = &TargetFBX;
	Serialized = FBXPtr->SerializedFlag;
	CreateBuffer(GlobalSystem);

	TotalTime = FBXPtr->TotalTime;
	StartTime = FBXPtr->StartTime;

	if (!Serialized)
		CurrentAnimationName = FBXPtr->CurrentAnimationStackName;

	DestDelay = 1.0 / (float)AnimationExtractFrame;
}

void FBX::SelectAnimation(std::string AnimationName) {
	if (!Serialized) {
		FbxAnimStack* Stack = FBXPtr->Scene->FindMember<FbxAnimStack>(AnimationName.c_str());

		if (Stack) {
			CurrentAnimationName = AnimationName;
			FbxTimeSpan span = Stack->GetLocalTimeSpan();
			FbxTime start = span.GetStart();
			FbxTime end = span.GetStop();
			TotalTime = (end - start).GetSecondDouble();
			CurrentTime = 0.0;
		}
	}

	else {
		auto Found = FBXPtr->SerializedAnimationStacks.find(AnimationName);
		if (Found != FBXPtr->SerializedAnimationStacks.end()) {
			CurrentAnimationName = AnimationName;
			StartTime = Found->second.StartTime;
			TotalTime = Found->second.EndTime;
			CurrentTime = StartTime;
		}
	}
}

void FBX::StopAnimationUpdate() {
	Running = false;
}

void FBX::ResumeAnimationUpdate() {
	Running = true;
}

void FBX::SetSpeed(float Speed) {
	CurrentSpeed = Speed;
}

void FBX::UpdateAnimation(float Delta, bool Inplace) {
	if (!Running)
		return;

	CurrentTime += Delta * CurrentSpeed;

	if (CurrentTime >= TotalTime) {
		float OverTime = CurrentTime - TotalTime;

		if (!Serialized)
			CurrentTime = OverTime;
		else
			CurrentTime = StartTime + OverTime;
	}

	CurrentDelay += Delta;
	if (CurrentDelay >= DestDelay) {
		CurrentDelay -= DestDelay;

		std::string SearchName;
		if (!FBXPtr->SerializedFlag)
			SearchName = CurrentAnimationName;
		else
			SearchName = FBXPtr->AnimationStackNames[0];

		for (int M = 0; M < MeshCount; M++) {
			auto FoundFrames = FBXPtr->MeshPart[M]->PrecomputedBoneMatrices.find(SearchName);
			if (FoundFrames == FBXPtr->MeshPart[M]->PrecomputedBoneMatrices.end())
				break;

			CurrentFrame = std::clamp(static_cast<int>(CurrentTime * AnimationExtractFrame), 0, (int)FoundFrames->second.size() - 1);

			if (PrevFrame != CurrentFrame) {
				if (M == 0 && Inplace)
					RootFrame = FoundFrames->second;

				FBXPtr->MeshPart[M]->UpdateSkinning(*FBXPtr, FoundFrames->second[CurrentFrame], PositionMapped[M], NormalMapped[M], CurrentTime);
				FrameUpdateState = true;
			}

			else
				break;
		}

		if (FrameUpdateState) {
			if (Inplace)
				InplaceDelta = GetRootMoveDelta(RootFrame, true);
			else
				InplaceDelta = XMFLOAT3(0.0, 0.0, 0.0);
			PrevFrame = CurrentFrame;
			FrameUpdateState = false;
		}
	}
}

XMFLOAT3 FBX::GetRootMoveDelta(std::vector<BoneFrame>& BoneFrame, bool InPlace) {
	if (BoneFrame.empty())
		return XMFLOAT3(0.0, 0.0, 0.0);

	int FirstFrame;

	if (!Serialized)
		FirstFrame = 0;
	else
		FirstFrame = std::clamp(static_cast<int>(StartTime * AnimationExtractFrame), 0, (int)BoneFrame.size() - 1);

	// 첫 번째 본(루트 본)을 기준으로 이동량 측정
	XMMATRIX currentMatrix = BoneFrame[CurrentFrame][0];
	XMMATRIX previousMatrix = BoneFrame[FirstFrame][0];

	XMVECTOR currentT = currentMatrix.r[3];
	XMVECTOR previousT = previousMatrix.r[3];
	XMVECTOR delta = currentT - previousT;

	if (InPlace)
		delta = XMVectorSet(XMVectorGetX(delta), 0.0f, XMVectorGetZ(delta), 0.0f);

	XMFLOAT3 result{};
	XMStoreFloat3(&result, delta);

	return result;
}

std::string FBX::GetCurrentAnimation() {
	return CurrentAnimationName;
}

void FBX::ResetAnimation() {
	if (!Serialized)
		CurrentTime = 0.0;
	else
		CurrentTime = StartTime;
}

size_t FBX::GetMeshCount() {
	return MeshCount;
}

void FBX::Render(int Index) {
	FBXPtr->MeshPart[Index]->Render(GlobalCommandList, VertexBufferViews[Index]);
}

void FBX::ApplyAnimation() {

}

XMFLOAT3 FBX::GetInplaceDelta() {
	return InplaceDelta;
}
///////////////////////////////////////// private

// 원본 FBX와 동일 사양으로 버퍼를 맞춘다.
void FBX::CreateBuffer(DeviceSystem& System) {
	MeshCount = FBXPtr->MeshPart.size();

	PositionBuffer.resize(MeshCount);
	PositionUploadBuffer.resize(MeshCount);
	NormalBuffer.resize(MeshCount);
	NormalUploadBuffer.resize(MeshCount);
	TextureCoordBuffer.resize(MeshCount);
	TextureCoordUploadBuffer.resize(MeshCount);
	VertexBufferViews.resize(MeshCount);
	PositionMapped.resize(MeshCount);
	NormalMapped.resize(MeshCount);

	for (int M = 0; M < MeshCount; M++) {
		Mesh* Curr = FBXPtr->MeshPart[M];

		PositionBuffer[M] = ::CreateBufferResource(System.Device, System.CmdList,
			Curr->Position, sizeof(XMFLOAT3) * Curr->Vertices,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &PositionUploadBuffer[M]);

		NormalBuffer[M] = ::CreateBufferResource(System.Device, System.CmdList,
			Curr->Normal, sizeof(XMFLOAT3) * Curr->Vertices,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &NormalUploadBuffer[M]);

		TextureCoordBuffer[M] = ::CreateBufferResource(System.Device, System.CmdList,
			Curr->TextureCoords, sizeof(XMFLOAT2) * Curr->Vertices,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &TextureCoordUploadBuffer[M]);

		VertexBufferViews[M] = new D3D12_VERTEX_BUFFER_VIEW[3];

		VertexBufferViews[M][0].BufferLocation = PositionBuffer[M]->GetGPUVirtualAddress();
		VertexBufferViews[M][0].StrideInBytes = sizeof(XMFLOAT3);
		VertexBufferViews[M][0].SizeInBytes = sizeof(XMFLOAT3) * Curr->Vertices;

		VertexBufferViews[M][1].BufferLocation = NormalBuffer[M]->GetGPUVirtualAddress();
		VertexBufferViews[M][1].StrideInBytes = sizeof(XMFLOAT3);
		VertexBufferViews[M][1].SizeInBytes = sizeof(XMFLOAT3) * Curr->Vertices;

		VertexBufferViews[M][2].BufferLocation = TextureCoordBuffer[M]->GetGPUVirtualAddress();
		VertexBufferViews[M][2].StrideInBytes = sizeof(XMFLOAT2);
		VertexBufferViews[M][2].SizeInBytes = sizeof(XMFLOAT2) * Curr->Vertices;

		D3D12_RANGE Read{ 0, 0 };
		PositionBuffer[M]->Map(0, &Read, &PositionMapped[M]);
		memcpy(PositionMapped[M], Curr->Position, sizeof(XMFLOAT3) * Curr->Vertices);

		NormalBuffer[M]->Map(0, &Read, &NormalMapped[M]);
		memcpy(NormalMapped[M], Curr->Normal, sizeof(XMFLOAT3) * Curr->Vertices);
	}
}

void FBX::ReleaseBuffer() {
	for (int M = 0; M < MeshCount; M++) {
		if (PositionBuffer[M])
			PositionBuffer[M]->Release();

		if (PositionUploadBuffer[M])
			PositionUploadBuffer[M]->Release();

		if (NormalBuffer[M])
			NormalBuffer[M]->Release();

		if (NormalUploadBuffer[M])
			NormalUploadBuffer[M]->Release();

		if (TextureCoordBuffer[M])
			TextureCoordBuffer[M]->Release();

		if (TextureCoordUploadBuffer[M])
			TextureCoordUploadBuffer[M]->Release();

		if (VertexBufferViews[M])
			delete[] VertexBufferViews[M];
	}
}
