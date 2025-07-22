#include "MeshUtil.h"
#include "Config.h"

Mesh* FBX::operator [] (int Index) {
	return FBXPtr->MeshPart[Index];
}


FBX::FBX() {}

FBX::FBX(FBXMesh& TargetFBXMesh, bool StopState) {
	SelectFBXMesh(TargetFBXMesh, StopState);
}

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
	OffsetTime = FBXPtr->OffsetTime;

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
			double Start = Found->second.StartTime;
			double End = Found->second.EndTime;

			StartTime = Start - (double)OffsetTime;
			TotalTime = End - (double)OffsetTime;
			CurrentTime = StartTime;
		}
	}

	CurrentEndCount = 0;
	PrevEndCount = 0;
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

// 하나의 키프레임이 끝날 때 true를 반환한다.
bool FBX::GetAnimationEndState() {
	if (PrevEndCount != CurrentEndCount) {
		PrevEndCount = CurrentEndCount;
		return true;
	}

	return false;
}

float FBX::GetCurrentAnimationTime() {
	return TotalTime;
}

float FBX::GetCurrentPlayTime() {
	return CurrentTime;
}

void FBX::SetCurrentPlayTime(float Time) {
	CurrentTime = Time;
}

// 애니메이션 키프레임 재생 시간이 특정 시간을 지나면 true를 리턴한다.
bool FBX::GetTimeSectionPassed(float Time) {
	if (CurrentTime >= Time) 
		return true;
		
	return false;
}

void FBX::UpdateAnimation(float Delta, bool Inplace, bool OnlyDeltaUpdate) {
	if (!Running)
		return;

	CurrentTime += Delta * CurrentSpeed;

	if (CurrentTime >= TotalTime) {
		float OverTime = CurrentTime - TotalTime;

		if (!Serialized)
			CurrentTime = OverTime;
		else
			CurrentTime = StartTime + OverTime;

		CurrentEndCount++;
	}

	CurrentDelay += Delta;
	if (CurrentDelay >= DestDelay) {
		DataReadCount++;

		if (OnlyDeltaUpdate)
			return;

		std::string SearchName;
		if (!FBXPtr->SerializedFlag)
			SearchName = CurrentAnimationName;
		else
			SearchName = FBXPtr->AnimationStackNames[0];

		for (int M = 0; M < MeshCount; M++) {
			auto FoundFrames = FBXPtr->MeshPart[M]->PrecomputedBoneMatrices.find(SearchName);
			if (FoundFrames == FBXPtr->MeshPart[M]->PrecomputedBoneMatrices.end())
				continue;

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
			if (Inplace) {
				InplaceDelta = GetRootMoveDelta(RootFrame, true);
				//std::cout << InplaceDelta.x << " " << InplaceDelta.y << " " << InplaceDelta.z << std::endl;
			}
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
	if (!Serialized) {
		CurrentDelay = 0.0;
		CurrentTime = 0.0;
	}
	else {
		CurrentDelay = 0.0;
		CurrentTime = StartTime;
	}
}

size_t FBX::GetMeshCount() {
	return MeshCount;
}

void FBX::Render(int Index) {
	FBXPtr->MeshPart[Index]->Render(GlobalCommandList, VertexBufferViews[Index]);
}


XMFLOAT3 FBX::GetInplaceDelta(const XMFLOAT3& Scale) {
	XMFLOAT3 ReturnValue{};
	ReturnValue.x = InplaceDelta.x * Scale.x;
	ReturnValue.y = InplaceDelta.y * Scale.y;
	ReturnValue.z = InplaceDelta.z * Scale.z;
	return ReturnValue;
}

XMFLOAT3 FBX::GetNodeRotation(int NodeIndex) {
	Mesh* MeshNode = FBXPtr->MeshPart[NodeIndex];
	auto FBXNode = MeshNode->FbxNodePtr;
	FbxTime Time;
	Time.SetSecondDouble(CurrentTime);
	FbxAMatrix NodeTransform = FBXNode->EvaluateGlobalTransform(Time);
	FbxVector4 RotationEuler = NodeTransform.GetR();

	float DegreeX = static_cast<float>(RotationEuler[0]);
	float DegreeY = static_cast<float>(RotationEuler[1]);
	float DegreeZ = static_cast<float>(RotationEuler[2]);

	return XMFLOAT3(DegreeX, DegreeY, DegreeZ);
}

void FBX::ApplyAnimation() {}

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
