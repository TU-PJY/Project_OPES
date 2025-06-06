#include "MeshUtil.h"

FBX::FBX(DeviceSystem& System, FBXMesh& TargetFBX, bool StopState) {
	if (InitState)
		return;

	FBXPtr = &TargetFBX;
	Serialized = FBXPtr->SerilaizedFlag;
	SelectAnimation(FBXPtr->CurrentAnimationStackName);
	CreateBuffer(System);

	if (StopState)
		StopAnimationUpdate();
}

FBX::FBX() {}

FBX::~FBX() {
	ReleaseBuffer();
}

void FBX::SelectFBXMesh(DeviceSystem& System, FBXMesh& TargetFBX) {
	if (InitState)
		return;

	FBXPtr = &TargetFBX;
	Serialized = FBXPtr->SerilaizedFlag;
	SelectAnimation(FBXPtr->CurrentAnimationStackName);
	CreateBuffer(System);
}

void FBX::SelectAnimation(std::string AnimationName) {
	if (!Serialized) {
		FbxAnimStack* Stack = FBXPtr->Scene->FindMember<FbxAnimStack>(AnimationName.c_str());

		if (Stack) {
			FBXPtr->Scene->SetCurrentAnimationStack(Stack);
			FBXPtr->CurrentAnimationStackName = AnimationName;
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
			FBXPtr->CurrentAnimationStackName = AnimationName;
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

void FBX::UpdateAnimation(float Delta) {
	if (!Running)
		return;

	CurrentTime += Delta * CurrentSpeed;

	//	if(UpdateLimit > 0)
	CurrentDelay += Delta;

	if (CurrentTime >= TotalTime) {
		float OverTime = CurrentTime - TotalTime;

		if (!Serialized)
			CurrentTime = OverTime;
		else
			CurrentTime = StartTime + OverTime;
	}
}

XMFLOAT3 FBX::GetRootMoveDelta(bool InPlace) {
	if (!FBXPtr->GlobalRootNode)
		return XMFLOAT3(0.0, 0.0, 0.0);

	FbxTime Time;
	Time.SetSecondDouble(CurrentTime);
	FbxAMatrix Matrix = FBXPtr->GlobalRootNode->EvaluateGlobalTransform(Time);
	FbxVector4 T = Matrix.GetT();

	return XMFLOAT3(T[0], T[1], T[2]);
}

std::string FBX::GetCurrentAnimation() {
	return CurrentAnimationName;
}

void FBX::ApplyAnimation() {
	//if (UpdateLimit > 0) {
	if (CurrentDelay >= UpdateLimit) {
		float OverTime = CurrentDelay - UpdateLimit;
		CurrentDelay = OverTime;

		if (!Serialized) {
			if (CurrentAnimationName.compare(FBXPtr->CurrentAnimationStackName) != 0) {
				FbxAnimStack* Stack = FBXPtr->Scene->FindMember<FbxAnimStack>(CurrentAnimationName.c_str());
				if (Stack) {
					FBXPtr->Scene->SetCurrentAnimationStack(Stack);
					FBXPtr->CurrentAnimationStackName = CurrentAnimationName;
				}
			}
		}

		for (int M = 0; M < MeshCount; M++)
			FBXPtr->MeshPart[M]->UpdateSkinning(PositionMapped[M], NormalMapped[M], CurrentTime);
	}
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

void FBX::Render(ID3D12GraphicsCommandList* CmdList, int Index) {
	FBXPtr->MeshPart[Index]->Render(CmdList, VertexBufferViews[Index]);
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
