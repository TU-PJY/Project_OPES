#pragma once
#include "MeshUtil.h"
#include "TextureUtil.h"
#include "ImageShader.h"
#include "ObjectShader.h"
#include "BoundboxShader.h"
#include "LineShader.h"

struct ObjectStruct {
	XMFLOAT3 Position;
	XMFLOAT3 Size;
	float Rotation;
	int Index;
};

// ���� ī�޶� fov �� ������
extern float global_fov_offset;

extern bool player_enter;
extern unsigned int enter_player_id;


/////////////////////////////////////////////////////////////////////////////////
// �Ž� ���ҽ��� �ش� Ŭ���� �ȿ� ����
typedef struct {
	// Map1 �Ž�
	Mesh* RockMesh;
	Mesh* LakeMesh;
	Mesh* LakeRockMesh[3];
	Mesh* TerrainMesh1;
	Mesh* MapObjectMesh[5];
	Mesh* Mushroom[2];
	Mesh* Grass[2];
	Mesh* Flower[2];

	// Map2 �Ž�
	Mesh* WinterWall;
	Mesh* WinterIce[2];
	Mesh* WinterRock[3];

	// Map3  �Ž�
	Mesh* FloatingRock;
	Mesh* Volcano;
	Mesh* SmallVolcano;
	Mesh* DeadTree;
	Mesh* Map3Stone[2];
	Mesh* Crystal[3];

	// polygon scifi asset
	Mesh* machine_gun;
	Mesh* dot_machine_gun;

	// polygon scifi asset - heavy
	FBXMesh heavy_idle;
	FBXMesh heavy_move;
	FBXMesh heavy_shoot;
	FBXMesh heavy_death;

	Mesh* center_building;

	// gun flame
	Mesh* gun_flame;
	Mesh* gun_flame_back;

	// map1 monster
	FBXMesh scorpion;
}MeshResource;
extern MeshResource MESH;

/////////////////////////////////////////////////////////////////////////////////
// �ؽ�ó ���ҽ��� �ش� Ŭ���� �ȿ� ����
typedef struct {
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

	// polyson scifi asset
	Texture* scifi;

	// gun flame texture
	Texture* gun_flame;
	Texture* gun_flame_back;

	// map1 monster
	Texture* scorpion;
}TextureResource;
extern TextureResource TEX;

/////////////////////////////////////////////////////////////////////////////////

extern Object_Shader* ObjectShader;
extern Boundbox_Shader* BoundboxShader;
extern Image_Shader* ImageShader;
extern Line_Shader* LineShader;

/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////// �⺻ ���ҽ�
// �Ž�, �ؽ�ó �ε� �� �����͸� �����ϴ� ����
// �Ѳ����� ���ε� ���۸� ������
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
extern SystemResource SysRes;

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

// �ִϸ��̼� FBX ���� �ε�� �Լ�
inline void LoadAnimatedFBX(DeviceSystem& System, FBXMesh& TargetMesh, std::string Directory, std::string jsonFile="") {
	if (fbxUtil.LoadAnimatedFBXFile(Directory.c_str(), TargetMesh)) {
		fbxUtil.TriangulateAnimatedScene();
		fbxUtil.GetAnimatedVertexData(System);
		fbxUtil.ProcessAnimation();
		//fbxUtil.PrintAnimationStackNames();
		fbxUtil.EnumerateAnimationStacks();
		fbxUtil.ClearVertexVector();
	}

	// ����ȭ �ִϸ��̼� �����͸� ���� ��� ������ �ִϸ��̼� Ű�������� �����Ѵ�.
	if (!jsonFile.empty()) {
		TargetMesh.SerilaizedFlag = true;
		fbxUtil.CreateAnimationStacksFromJSON(jsonFile, TargetMesh);
	}
}

// �ִϸ��̼��� ���� FBX ���� �ε�� �Լ�
inline void LoadSingleStaticFBX(DeviceSystem& System, Mesh*& TargetMesh, std::string Directory) {
	if (fbxUtil.LoadStaticFBXFile(Directory.c_str(), TargetMesh)) {
		fbxUtil.TriangulateStaticScene();
		fbxUtil.GetSingleStaticVertexData();
		TargetMesh->CreateFBXMesh(System.Device, System.CmdList, fbxUtil.GetVertexVector());
		LoadedMeshList.emplace_back(TargetMesh);
		fbxUtil.ClearVertexVector();
	}
}

// �ִϸ��̼��� ���� ���� FBX ���� �ε�� �Լ�
inline void LoadMultiStaticFBX(DeviceSystem& System, Mesh*& TargetMesh, std::string Directory) {
	if (fbxUtil.LoadMultiStaticFBXFile(Directory.c_str(), TargetMesh)) {
		fbxUtil.TriangulateMultiStaticScene();
		fbxUtil.GetMultiStaticVertexData();
		TargetMesh->CreateFBXMesh(System.Device, System.CmdList, fbxUtil.GetVertexVector());
		LoadedMeshList.emplace_back(TargetMesh);
		fbxUtil.ClearVertexVector();
	}
}

// TEXTURE_TYPE_WIC, D3D12_FILTER_MIN_MAG_MIP_POINT�� ����Ʈ
inline void LoadTexture(DeviceSystem& System, Texture*& TexturePtr, wchar_t* Directory, int Type=TEXTURE_TYPE_WIC, D3D12_FILTER FilterOption=D3D12_FILTER_MIN_MAG_MIP_POINT) {
	TexturePtr = new Texture(System.Device, System.CmdList, Directory, Type, FilterOption);
	LoadedTextureList.emplace_back(TexturePtr);
}