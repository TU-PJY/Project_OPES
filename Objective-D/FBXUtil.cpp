#include <fbxsdk.h> 
#include <json.hpp>
#include "MeshUtil.h"


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
	Importer->Destroy();

	return true;
}

bool FBXUtil::LoadMultiStaticFBXFile(std::string FilePath, Mesh*& TargetMesh) {
	return LoadStaticFBXFile(FilePath.c_str(), TargetMesh);
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
	Importer->Destroy();

	FbxNode* RootNode = Scene->GetRootNode();
	for (int i = 0; i < RootNode->GetChildCount(); ++i) {
		if (RootNode->GetChild(i)->GetSkeleton()) {
			TargetMesh.GlobalRootNode = RootNode->GetChild(i);
			break;
		}
	}

	return true;
}

bool FBXUtil::TriangulateStaticScene() {
	FbxGeometryConverter GeometryConverter(Manager);
	bool Result = GeometryConverter.Triangulate(StaticScene, true);

	if (!Result) {
		std::cerr << "Error: Triangulation failed!\n";
		return false;
	}
	return true;
}

bool FBXUtil::TriangulateMultiStaticScene() {
	return TriangulateStaticScene();
}

bool FBXUtil::TriangulateAnimatedScene() {
	FbxGeometryConverter GeometryConverter(Manager);
	bool Result = GeometryConverter.Triangulate(MeshPtr->Scene, true);

	if (!Result) {
		std::cerr << "Error: Triangulation failed!\n";
		return false;
	}
	return true;
}

void FBXUtil::GetSingleStaticVertexData() {
	FbxNode* RootNode = StaticScene->GetRootNode();

	if (RootNode) {
		for (int i = 0; i < RootNode->GetChildCount(); ++i)
			ProcessSingleStaticNode(RootNode->GetChild(i));
	}
}

void FBXUtil::GetMultiStaticVertexData() {
	FbxNode* RootNode = StaticScene->GetRootNode();

	if (RootNode) {
		for (int i = 0; i < RootNode->GetChildCount(); ++i)
			ProcessMultiStaticNode(RootNode->GetChild(i));
	}
}

void FBXUtil::GetAnimatedVertexData(DeviceSystem& System) {
	FbxNode* RootNode = MeshPtr->Scene->GetRootNode();

	if (RootNode) {
		for (int i = 0; i < RootNode->GetChildCount(); ++i)
			ProcessAnimatedNode(RootNode->GetChild(i), System);
	}
}

void FBXUtil::ProcessSingleStaticNode(FbxNode* Node) {
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
		ProcessSingleStaticNode(Node->GetChild(i));
}

void FBXUtil::ProcessMultiStaticNode(FbxNode* Node) {
	FbxMesh* FMesh = Node->GetMesh();
	if (FMesh) {
		FbxVector4* ControlPoints = FMesh->GetControlPoints();
		int PolygonCount = FMesh->GetPolygonCount();
		int NumControlPoint{};
		int VertexIndex{};

		FbxAMatrix GlobalTransform = Node->EvaluateGlobalTransform();
		FbxAMatrix NormalTransform = GlobalTransform.Inverse().Transpose();

		for (int PolyIndex = 0; PolyIndex < PolygonCount; PolyIndex++) {
			int VertexCountInPolygon = FMesh->GetPolygonSize(PolyIndex);

			for (int V = 0; V < VertexCountInPolygon; V++) {
				int ControlPointIndex = FMesh->GetPolygonVertex(PolyIndex, V);
				if (ControlPointIndex < 0) continue;

				NumControlPoint = ControlPointIndex;

				FbxVector4 LocalPosition = ControlPoints[ControlPointIndex];
				FbxVector4 WorldPosition = GlobalTransform.MultT(LocalPosition);
				FbxVector4 Normal(0, 0, 0, 0);

				bool HasNormal = FMesh->GetPolygonVertexNormal(PolyIndex, V, Normal);

				if (HasNormal) {
					FbxVector4 row0 = GlobalTransform.GetRow(0);
					FbxVector4 row1 = GlobalTransform.GetRow(1);
					FbxVector4 row2 = GlobalTransform.GetRow(2);

					FbxVector4 RotatedNormal;
					RotatedNormal[0] = Normal[0] * row0[0] + Normal[1] * row0[1] + Normal[2] * row0[2];
					RotatedNormal[1] = Normal[0] * row1[0] + Normal[1] * row1[1] + Normal[2] * row1[2];
					RotatedNormal[2] = Normal[0] * row2[0] + Normal[1] * row2[1] + Normal[2] * row2[2];
					RotatedNormal[3] = 0.0;

					RotatedNormal.Normalize();
					Normal = RotatedNormal;
				}

				FbxVector2 UV(0, 0);
				const char* UVSetName = nullptr;
				if (FMesh->GetElementUVCount() > 0) {
					FbxLayerElementUV* UVElement = FMesh->GetElementUV(0);
					if (UVElement) UVSetName = UVElement->GetName();
				}
				bool Unmapped = false;
				bool HasUV = FMesh->GetPolygonVertexUV(PolyIndex, V, UVSetName, UV, Unmapped);

				FBXVertex Vertex{};
				Vertex.px = static_cast<float>(WorldPosition[0]);
				Vertex.py = static_cast<float>(WorldPosition[1]);
				Vertex.pz = static_cast<float>(WorldPosition[2]);
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
		ProcessMultiStaticNode(Node->GetChild(i));
}

void FBXUtil::ProcessAnimatedNode(FbxNode* Node, DeviceSystem& System) {
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
		SelectAnimation(*MeshPtr, MeshPtr->AnimationStackNames[0]);
	}
}

void FBXUtil::SelectAnimation(FBXMesh& TargetMesh, const std::string& AnimationName) {
	if (!TargetMesh.SerializedFlag) {
		int Count = TargetMesh.Scene->GetSrcObjectCount<FbxAnimStack>();

		for (int i = 0; i < Count; ++i) {
			FbxAnimStack* Stack = TargetMesh.Scene->GetSrcObject<FbxAnimStack>(i);

			if (Stack && AnimationName == Stack->GetName()) {
				TargetMesh.Scene->SetCurrentAnimationStack(Stack);
				TargetMesh.CurrentAnimationStackName = AnimationName;
				GetAnimationPlayTime(TargetMesh, AnimationName);
			}
		}
	}
	else {
		auto Found = TargetMesh.SerializedAnimationStacks.find(AnimationName);
		if (Found != TargetMesh.SerializedAnimationStacks.end()) {
			TargetMesh.CurrentAnimationStackName = AnimationName;
			TargetMesh.StartTime = Found->second.StartTime;
			TargetMesh.TotalTime = Found->second.EndTime;
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

void FBXUtil::PrecomputeBoneMatrices(FBXMesh& TargetMesh, const std::string& AnimationName, float fps) {
	SelectAnimation(TargetMesh, AnimationName);              // 애니메이션 스택 설정
	GetAnimationPlayTime(TargetMesh, AnimationName);         // StartTime, EndTime 설정

	float duration = TargetMesh.TotalTime;
	int frameCount = static_cast<int>(duration * fps);

	// 각 Mesh별로 boneFrame 벡터 미리 준비
	for (auto m : TargetMesh.MeshPart) {
		std::vector<BoneFrame> boneFrames;
		boneFrames.resize(frameCount);

		for (int i = 0; i < frameCount; ++i) {
			float time = TargetMesh.StartTime + (i / fps);
			std::vector<XMMATRIX> matrices;

			// 이 mesh에 해당하는 본만 계산
			GetBoneMatricesFromScene(m, time, matrices);

			boneFrames[i] = std::move(matrices);
		}

		m->PrecomputedBoneMatrices[AnimationName] = std::move(boneFrames);
	}
}

void FBXUtil::CreateAnimationStacksFromJSON(std::string jsonFile, FBXMesh& TargetMesh) {
	std::ifstream File(jsonFile, std::ios::binary);
	if (!File) {
		std::cerr << "파일을 열 수 없습니다." << std::endl;
		return;
	}
	nlohmann::json JSON;

	File >> JSON;

	std::string KeyFrameName{};
	SerializedAnimationInfo Info{};

	bool FirstLoad = true;

	for (const auto& clip : JSON["clips"]) {
		KeyFrameName = clip["name"].get<std::string>();
		Info.StartTime = clip["startTime"].get<float>();
		Info.EndTime = clip["endTime"].get<float>();
		TargetMesh.SerializedAnimationStacks.emplace(std::pair(KeyFrameName, Info));

		//std::cout << KeyFrameName << " [" << Info.StartTime << " - " << Info.EndTime << "] @ \n";

		if (FirstLoad) {
			SelectAnimation(TargetMesh, KeyFrameName);
			FirstLoad = false;
		}
	}
}

XMFLOAT3 FBXUtil::GetRootMoveDelta(FBXMesh& TargetMesh, bool InPlace) {
	if (!TargetMesh.GlobalRootNode)
		return XMFLOAT3(0.0, 0.0, 0.0);

	FbxTime Time;
	Time.SetSecondDouble(TargetMesh.CurrentTime);
	FbxAMatrix Matrix = TargetMesh.GlobalRootNode->EvaluateGlobalTransform(Time);
	FbxVector4 T = Matrix.GetT();

	return XMFLOAT3(T[0], T[1], T[2]);
}