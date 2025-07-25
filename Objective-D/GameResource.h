#pragma once
#include "MeshUtil.h"
#include "TextureUtil.h"
#include "ImageShader.h"
#include "ObjectShader.h"
#include "BoundboxShader.h"
#include "LineShader.h"
#include "Config.h"
#include "CollisionUtil.h"
#include "TerrainUtil.h"

struct ObjectStruct {
	XMFLOAT3 Position;
	XMFLOAT3 Size;
	float Rotation;
	int Index;
};

extern bool player_enter;
extern unsigned int enter_player_id;

extern DeviceSystem LoadSystem;

enum GlobalEnum : unsigned int {
	// 엔지니어 설치물 타입
	CONSTRUCT_TURRET,
	CONSTRUCT_WALL,
	CONSTRUCT_HEAL
};

// 프로젝트 전역에서 사용하는 변수들
typedef struct {
	// 디펜스 모드에서 사용할 남은 적 수  및 디펜스 모드 상태 여부
	int DefenseEnemyRemained;
	bool DefenseState;

	// 카메라 FOV 오프셋
	float offsetFOV;

	// 전역에서 사용되는 맵 객체 이름
	std::string mapName;

	// 전역에서 사용하는 맵 오브젝트 OOBB 데이터
	std::vector<OOBB> mapOOBBdata;

	// 전역에서 사용하는 맵 터레인 객체
	TerrainUtil mapTerrain;

	// 전역 플레이어 인디케이터 객체
	LPVOID otherIndicator;

	// 서버 사용 여부
	bool useServer;

	// 현재 접속한 나의 ID
	unsigned int myID;

}GlobalValue;
extern GlobalValue GLOBAL;

/////////////////////////////////////////////////////////////////////////////////
// 매쉬 리소스는 해당 클래스 안에 선언
typedef struct {
	// Map1 매쉬
	Mesh* RockMesh;
	Mesh* LakeMesh;
	Mesh* LakeRockMesh[3];
	Mesh* TerrainMesh1;
	Mesh* MapObjectMesh[5];
	Mesh* Mushroom[2];
	Mesh* Grass[2];
	Mesh* Flower[2];

	// Map2 매쉬
	Mesh* WinterWall;
	Mesh* WinterIce[2];
	Mesh* WinterRock[3];

	// Map3  매쉬
	Mesh* FloatingRock;
	Mesh* Volcano;
	Mesh* SmallVolcano;
	Mesh* DeadTree;
	Mesh* Map3Stone[2];
	Mesh* Crystal[3];

	// explosion effect
	FBXMesh explosion;

	// polygon scifi asset
	Mesh* machine_gun;
	Mesh* dot_machine_gun;
	Mesh* shotgun;
	Mesh* dot_shotgun;
	Mesh* dmr;
	Mesh* grenade;

	Mesh* beacon;
	Mesh* barrier;

	// polygon scifi asset - heavy
	FBXMesh heavyIdle;
	FBXMesh heavyMove;
	FBXMesh heavyShoot;
	FBXMesh heavyDeath;

	Mesh* center_building;

	// turret asset
	Mesh* turretBottom;
	Mesh* turretTorso;
	Mesh* turretHead;

	// gun flame
	Mesh* gun_flame;
	Mesh* gun_flame_back;

	Mesh* turretFlame;
	Mesh* turretFlameBack;

	// monster
	FBXMesh plantMonster;
	FBXMesh scorpion;

	FBXMesh treant[4];
	FBXMesh troll;

	FBXMesh gazer;
	FBXMesh imp;

}MeshResource;
extern MeshResource MESH;

/////////////////////////////////////////////////////////////////////////////////
// 텍스처 리소스는 해당 클래스 안에 선언
typedef struct {
	// font atlas
	Texture* fontAtlas[96];

	// skybox
	Texture* skyBox;
	Texture* skyBox2;

	// map1
	Texture* Palette1;
	Texture* Palette2;
	Texture* Palette3;
	Texture* ColorTex;
	
	// map2
	Texture* Map2Palette;
	Texture* Map2TerrainTex;
	Texture* IceTex;

	//map3
	Texture* Magma;
	Texture* Map3RockColor;
	Texture* Map3Palette;
	Texture* Volcano;

	// roadblock
	Texture* roadBlock;

	// polyson scifi asset
	Texture* scifi;

	// turret Assst
	Texture* turret;

	Texture* scope;

	// gun flame texture
	Texture* gun_flame;
	Texture* gun_flame_back;
	Texture* muzzle_particle;

	Texture* smoke[25];

	Texture* poisonBall;

	// map1 monster
	Texture* plantMonster;
	Texture* scorpion;

	// map2 monster
	Texture* treant;
	Texture* troll;

	// map3 monster
	Texture* gazer;
	Texture* imp;

	// UI
	Texture* UI_map1Enemy;
	Texture* UI_playerHit;
	Texture* UI_turretIcon;
	Texture* UI_beaconIcon;
	Texture* UI_barrierIcon;
	Texture* UI_shotgunIcon;
	Texture* UI_mgIcon;
	Texture* UI_dmrIcon;
	Texture* UI_grenadeIcon;
}TextureResource;
extern TextureResource TEX;

/////////////////////////////////////////////////////////////////////////////////

extern Object_Shader* ObjectShader;
extern Boundbox_Shader* BoundboxShader;
extern Image_Shader* ImageShader;
extern Line_Shader* LineShader;

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////// 기본 리소스
// 매쉬, 텍스처 로드 시 포인터를 저장하는 벡터
// 한꺼번에 업로드 버퍼를 삭제함
extern std::vector<Mesh*> LoadedMeshList;
extern std::vector<Texture*> LoadedTextureList;

class SystemResource {
public:
	Mesh* ImagePannel;
	Mesh* BillboardMesh;
	Mesh* SkyboxMesh;
	Mesh* BoundMesh;
	Mesh* BoundingSphereMesh;
};
extern SystemResource SYSRES;

void InitObjectShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device);
void InitImageShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device);
void InitBoundboxShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device);
void InitLineShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device);

void LoadSystemMesh(DeviceSystem& System);
void LoadMesh(DeviceSystem& System);
void LoadTexture(DeviceSystem& System);
void ClearUploadBuffer();

inline void ImportMesh(DeviceSystem& System, Mesh*& MeshPtr, char* Directory, int Type) {
	MeshPtr = new Mesh(System.Device, System.CmdList, Directory, Type);
	LoadedMeshList.emplace_back(MeshPtr);
}

inline void SavePrecomputedAnimation(const std::string& FilePath, const FBXMesh& TargetMesh) {
	std::ofstream out(FilePath, std::ios::binary);
	if (!out) {
		std::cerr << "Failed to open file for writing: " << FilePath << "\n";
		return;
	}

	uint32_t meshCount = static_cast<uint32_t>(TargetMesh.MeshPart.size());
	out.write(reinterpret_cast<const char*>(&meshCount), sizeof(meshCount));

	for (const Mesh* mesh : TargetMesh.MeshPart) {
		// Write node name
		uint32_t nodeNameLen = static_cast<uint32_t>(mesh->NodeName.size());
		out.write(reinterpret_cast<const char*>(&nodeNameLen), sizeof(nodeNameLen));
		out.write(mesh->NodeName.c_str(), nodeNameLen);

		uint32_t animCount = static_cast<uint32_t>(mesh->PrecomputedBoneMatrices.size());
		out.write(reinterpret_cast<const char*>(&animCount), sizeof(animCount));

		for (const auto& [animName, frames] : mesh->PrecomputedBoneMatrices) {
			uint32_t animNameLen = static_cast<uint32_t>(animName.size());
			out.write(reinterpret_cast<const char*>(&animNameLen), sizeof(animNameLen));
			out.write(animName.c_str(), animNameLen);

			uint32_t frameCount = static_cast<uint32_t>(frames.size());
			uint32_t boneCount = static_cast<uint32_t>(frames[0].size());
			out.write(reinterpret_cast<const char*>(&frameCount), sizeof(frameCount));
			out.write(reinterpret_cast<const char*>(&boneCount), sizeof(boneCount));

			for (const auto& frame : frames)
				out.write(reinterpret_cast<const char*>(frame.data()), sizeof(XMMATRIX) * boneCount);
		}
	}
}

inline void LoadPrecomputedAnimation(FBXMesh& TargetMesh, const std::string& FilePath) {
	if (AnimationDataExtractMode)
		return;

	std::ifstream in(FilePath, std::ios::binary);
	if (!in) {
		std::cerr << "Failed to open file for reading: " << FilePath << "\n";
		return;
	}

	uint32_t meshCount;
	in.read(reinterpret_cast<char*>(&meshCount), sizeof(meshCount));
	if (meshCount != TargetMesh.MeshPart.size()) {
		std::cerr << "Mismatch in mesh count!\n";
		return;
	}

	for (Mesh* mesh : TargetMesh.MeshPart) {
		uint32_t nodeNameLen;
		in.read(reinterpret_cast<char*>(&nodeNameLen), sizeof(nodeNameLen));
		std::string nodeName(nodeNameLen, '\0');
		in.read(&nodeName[0], nodeNameLen);

		if (mesh->NodeName != nodeName) {
			std::cerr << "Node name mismatch: " << mesh->NodeName << " != " << nodeName << "\n";
			continue;
		}

		uint32_t animCount;
		in.read(reinterpret_cast<char*>(&animCount), sizeof(animCount));

		for (uint32_t i = 0; i < animCount; ++i) {
			uint32_t animNameLen;
			in.read(reinterpret_cast<char*>(&animNameLen), sizeof(animNameLen));
			std::string animName(animNameLen, '\0');
			in.read(&animName[0], animNameLen);

			uint32_t frameCount, boneCount;
			in.read(reinterpret_cast<char*>(&frameCount), sizeof(frameCount));
			in.read(reinterpret_cast<char*>(&boneCount), sizeof(boneCount));

			std::vector<BoneFrame> frames(frameCount, BoneFrame(boneCount));
			for (auto& frame : frames)
				in.read(reinterpret_cast<char*>(frame.data()), sizeof(XMMATRIX) * boneCount);

			mesh->PrecomputedBoneMatrices[animName] = std::move(frames);
		}
	}
}

// 애니메이션 FBX 파일 로드용 함수
// CreateMode가 true인 매쉬들만 애니메이션 데이터가 추출된다.
inline void LoadAnimatedFBX(FBXMesh& TargetMesh, const std::string& Directory, const std::string& AnimationDataFile, const std::string& jsonFile = "", bool CreateMode=false) {
	if (!CreateMode && AnimationDataExtractMode)
		return;
	
	if (fbxUtil.LoadAnimatedFBXFile(Directory.c_str(), TargetMesh)) {
		fbxUtil.TriangulateAnimatedScene();
		fbxUtil.GetAnimatedVertexData(LoadSystem);

		if(AnimationDataExtractMode)
			fbxUtil.ProcessAnimation();

		fbxUtil.EnumerateAnimationStacks();

		if (AnimationDataExtractMode)
			fbxUtil.PrintAnimationStackNames();

		fbxUtil.ClearVertexVector();
	}

	if (!jsonFile.empty()) {
		TargetMesh.SerializedFlag = true;
		fbxUtil.CreateAnimationStacksFromJSON(jsonFile, TargetMesh);
	}

	if(!AnimationDataExtractMode)
		LoadPrecomputedAnimation(TargetMesh, AnimationDataFile);

	else {
		// 최적화를 위해 애니메이션 행렬 데이터를 미리 계산한다.
		int StackCount = TargetMesh.Scene->GetSrcObjectCount<FbxAnimStack>();
		for (int i = 0; i < StackCount; ++i) {
			FbxAnimStack* Stack = TargetMesh.Scene->GetSrcObject<FbxAnimStack>(i);
			if (Stack)
				fbxUtil.PrecomputeBoneMatrices(TargetMesh, Stack->GetName(), AnimationExtractFrame);
		}

		std::string FolderCreateName = "Resources//Animation Data";

		std::filesystem::path FilePath = AnimationDataFile;
		std::string FileName = FilePath.stem().string();
		std::string OutFileName = FolderCreateName + "//" + FileName;

		if(!std::filesystem::exists(FolderCreateName))
			std::filesystem::create_directory(FolderCreateName);

		SavePrecomputedAnimation(AnimationDataFile, TargetMesh);
		std::cout << "Saved Precomputed Animation Data To " << AnimationDataFile << ".\n";
	}
}

inline void SetAnimationOffset(FBXMesh& TargetFBX, float Offset) {
	fbxUtil.SetAnimationOffsetTime(TargetFBX, Offset);
}

// 애니메이션이 없는 FBX 파일 로드용 함수
// 애니메이션 추출모드를 활성화하면 건너뛴다.
inline void LoadSingleStaticFBX(Mesh*& TargetMesh, const std::string& Directory) {
	if (AnimationDataExtractMode)
		return;

	if (fbxUtil.LoadStaticFBXFile(Directory.c_str(), TargetMesh)) {
		fbxUtil.TriangulateStaticScene();
		fbxUtil.GetSingleStaticVertexData();
		TargetMesh->CreateFBXMesh(LoadSystem.Device, LoadSystem.CmdList, fbxUtil.GetVertexVector());
		LoadedMeshList.emplace_back(TargetMesh);
		fbxUtil.ClearVertexVector();
	}
}

// 애니메이션이 없는 다중 FBX 파일 로드용 함수
// 애니메이션 추출모드를 활성화하면 건너뛴다.
inline void LoadMultiStaticFBX(Mesh*& TargetMesh, const std::string& Directory) {
	if (AnimationDataExtractMode)
		return;

	if (fbxUtil.LoadMultiStaticFBXFile(Directory.c_str(), TargetMesh)) {
		fbxUtil.TriangulateMultiStaticScene();
		fbxUtil.GetMultiStaticVertexData();
		TargetMesh->CreateFBXMesh(LoadSystem.Device, LoadSystem.CmdList, fbxUtil.GetVertexVector());
		LoadedMeshList.emplace_back(TargetMesh);
		fbxUtil.ClearVertexVector();
	}
}

// TEXTURE_TYPE_WIC, D3D12_FILTER_MIN_MAG_MIP_POINT가 디폴트
// 애니메이션 추출모드를 활성화하면 건너뛴다.
inline void LoadTexture(Texture*& TexturePtr, wchar_t* Directory, int Type=TEXTURE_TYPE_WIC, D3D12_FILTER FilterOption= D3D12_FILTER_MIN_MAG_MIP_POINT) {
	if (AnimationDataExtractMode)
		return;
	
	TexturePtr = new Texture(LoadSystem.Device, LoadSystem.CmdList, Directory, Type, FilterOption);
	LoadedTextureList.emplace_back(TexturePtr);
}

inline void LoadCropTexture(Texture*& TexturePtr, wchar_t* Directory, int X, int Y, int SizeX, int SizeY, D3D12_FILTER FilterOption = D3D12_FILTER_MIN_MAG_MIP_POINT) {
	WICRect CropRect;
	CropRect.X = X;
	CropRect.Y = Y;
	CropRect.Width = SizeX;
	CropRect.Height = SizeY;

	TexturePtr = new Texture(LoadSystem.Device, LoadSystem.CmdList, Directory, CropRect, FilterOption);
	LoadedTextureList.emplace_back(TexturePtr);
}