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

/////////////////////////////////////////////////////////////////////////////////
// �Ž� ���ҽ��� �ش� Ŭ���� �ȿ� ����
class MeshResource {
public:
	// FBX �׽�Ʈ�� �Ž�

	//// Animated FBX ���� mesh
	//// FBX �Ž��� ���ο� �Ž��� �������� �� �����Ƿ� ���ο��� vector�� �ϰ� ó�� �� ������ �Ѵ�.
	FBXMesh man{};
	FBXMesh Lain{};
	FBXMesh steve{};

	// Map1 �Ž�
	Mesh* RockMesh;
	Mesh* LakeMesh;
	Mesh* LakeRockMesh[3]{};
	Mesh* TerrainMesh1{};
	Mesh* MapObjectMesh[5]{};
	Mesh* Mushroom[2]{};
	Mesh* Grass[2]{};
	Mesh* Flower[2]{};

	// Map2 �Ž�
	Mesh* WinterWall{};
	Mesh* WinterIce[2]{};
	Mesh* WinterRock[3]{};

	// Map3  �Ž�
	Mesh* FloatingRock{};
	Mesh* Volcano{};
	Mesh* SmallVolcano{};
	Mesh* DeadTree{};
	Mesh* Map3Stone[2]{};
	Mesh* Crystal[3]{};

	// FBX �ִϸ��̼� �����ֱ�� �Ž� 
	FBXMesh TestMesh;
};
extern MeshResource MESH;

/////////////////////////////////////////////////////////////////////////////////
// �ؽ�ó ���ҽ��� �ش� Ŭ���� �ȿ� ����
class TextureResource {
public:
	//// FBX �׽�Ʈ�� �Ž� �ؽ�ó
	Texture* man{};
	Texture* Lain{};
	Texture* steve{};

	// map1
	Texture* Palette1;
	Texture* Palette2;
	Texture* Palette3;
	Texture* ColorTex;
	
	// map2
	Texture* Map2Palette{};
	Texture* Map2TerrainTex{};
	Texture* IceTex;

	//map3
	Texture* Magma{};
	Texture* Map3RockColor{};
	Texture* Map3Palette{};
	Texture* Volcano{};

	Texture* TestTex;
};
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
extern std::vector<FBXMesh> LoadedFBXMeshList;

class SystemResource {
public:
	Mesh* ImagePannel;
	Mesh* BillboardMesh;
	Mesh* SkyboxMesh;
	Mesh* BoundMesh;
	Mesh* BoundingSphereMesh;
};
extern SystemResource SysRes;

void LoadShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device);
void LoadSystemMesh(DeviceSystem& System);
void LoadMesh(DeviceSystem& System);
void LoadTexture(DeviceSystem& System);
void ClearUploadBuffer();

inline void ImportMesh(DeviceSystem& System, Mesh*& MeshPtr, char* Directory, int Type) {
	MeshPtr = new Mesh(System.Device, System.CmdList, Directory, Type);
	LoadedMeshList.emplace_back(MeshPtr);
}

// �ִϸ��̼� FBX ���� �ε�� �Լ�
inline void LoadAnimatedFBX(DeviceSystem& System, FBXMesh& TargetMesh, char* Directory) {
	if (fbxUtil.LoadAnimatedFBXFile(Directory, TargetMesh)) {
		fbxUtil.TriangulateAnimatedScene();
		fbxUtil.GetAnimatedVertexData(System);
		fbxUtil.ProcessAnimation();
		//fbxUtil.PrintAnimationStackNames();
		fbxUtil.EnumerateAnimationStacks();
		LoadedFBXMeshList.emplace_back(TargetMesh);
	}
}

// �ִϸ��̼��� ���� FBX ���� �ε�� �Լ�
inline void LoadStaticFBX(DeviceSystem& System, Mesh*& TargetMesh, char* Directory) {
	if (fbxUtil.LoadStaticFBXFile(Directory, TargetMesh)) {
		fbxUtil.TriangulateStaticScene();
		fbxUtil.GetStaticVertexData();
		if (TargetMesh) {
			TargetMesh->CreateFBXMesh(System.Device, System.CmdList, fbxUtil.GetVertexVector());
			LoadedMeshList.emplace_back(TargetMesh);
		}
		fbxUtil.ClearVertexVector();
	}
}

// TEXTURE_TYPE_WIC, D3D12_FILTER_MIN_MAG_MIP_POINT�� ����Ʈ
inline void ImportTexture(DeviceSystem& System, Texture*& TexturePtr, wchar_t* Directory, int Type=TEXTURE_TYPE_WIC, D3D12_FILTER FilterOption=D3D12_FILTER_MIN_MAG_MIP_POINT) {
	TexturePtr = new Texture(System.Device, System.CmdList, Directory, Type, FilterOption);
	LoadedTextureList.emplace_back(TexturePtr);
}