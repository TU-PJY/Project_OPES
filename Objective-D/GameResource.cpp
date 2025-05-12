#include "GameResource.h"
#include <string>
// �� ������ ���ҽ��� �����Ѵ�.
// �⺻������ ���� ���ҽ��̸�, ResourceManager.h�� ���� extern ������ ��, �� ���Ͽ� �Ʒ��� ���� �����ϸ� �ȴ�.
// Scene::Init()���� ����ȴ�.

MeshResource MESH;
TextureResource TEX;
SystemResource SysRes;

// ���� ī�޶� fov �� ������
float global_fov_offset;

bool player_enter;
unsigned int enter_player_id;


// �Ž��� ���⼭ �ε��Ѵ�.
void LoadMesh(DeviceSystem& System) {
	// map1
	LoadSingleStaticFBX(System, MESH.RockMesh, "Resources//Models//map1//wall-rock.fbx");
	LoadSingleStaticFBX(System, MESH.LakeMesh, "Resources//Models//map1//lake.fbx");
	LoadSingleStaticFBX(System, MESH.TerrainMesh1, "Resources//Models//map1//terrain.fbx");

	for (int i = 0; i < 3; ++i) {
		std::string FileName = "Resources//Models//map1//lake-rock-";
		FileName += std::to_string(i + 1) + ".fbx";
		LoadSingleStaticFBX(System, MESH.LakeRockMesh[i], (char*)FileName.c_str());
	}

	for (int i = 0; i < 5; ++i) {
		std::string FileName = "Resources//Models//map1//map-object-";
		FileName += std::to_string(i + 1) + ".fbx";
		LoadSingleStaticFBX(System, MESH.MapObjectMesh[i], (char*)FileName.c_str());
	}

	LoadSingleStaticFBX(System, MESH.Grass[0], "Resources//Models//map1//grass-1.fbx");
	LoadSingleStaticFBX(System, MESH.Grass[1], "Resources//Models//map1//grass-2.fbx");
	LoadSingleStaticFBX(System, MESH.Flower[0], "Resources//Models//map1//flower-1.fbx");
	LoadSingleStaticFBX(System, MESH.Flower[1], "Resources//Models//map1//flower-2.fbx");
	LoadSingleStaticFBX(System, MESH.Mushroom[0], "Resources//Models//map1//mushroom-1.fbx");
	LoadSingleStaticFBX(System, MESH.Mushroom[1], "Resources//Models//map1//mushroom-2.fbx");


	// map2
	LoadSingleStaticFBX(System, MESH.WinterWall, "Resources//Models//map2//winter-cliff.fbx");

	for (int i = 0; i < 3; ++i) {
		std::string FileName = "Resources//Models//map2//winter-rock-" + std::to_string(i + 1) + ".fbx";
		LoadSingleStaticFBX(System, MESH.WinterRock[i], (char*)FileName.c_str());
	}

	for (int i = 0; i < 2; ++i) {
		std::string FileName = "Resources//Models//map2//winter-ice-" + std::to_string(i + 1) + ".fbx";
		LoadSingleStaticFBX(System, MESH.WinterIce[i], (char*)FileName.c_str());
	}

	// map3
	LoadSingleStaticFBX(System, MESH.FloatingRock, "Resources//Models//map3//map3-rock.fbx");
	LoadSingleStaticFBX(System, MESH.Volcano, "Resources//Models//map3//volcano.fbx");
	LoadSingleStaticFBX(System, MESH.SmallVolcano, "Resources//Models//map3//volcano-small.fbx");

	for (int i = 0; i < 2; i++) {
		std::string FileName = "Resources//Models//map3//map3-stone-" + std::to_string(i + 1) + ".fbx";
		LoadSingleStaticFBX(System, MESH.Map3Stone[i], (char*)FileName.c_str());
	}

	for (int i = 0; i < 3; ++i) {
		std::string FileName = "Resources//Models//map3//map3-crystal-" + std::to_string(i + 1) + ".fbx";
		LoadSingleStaticFBX(System, MESH.Crystal[i], (char*)FileName.c_str());
	}

	LoadSingleStaticFBX(System, MESH.DeadTree, "Resources//Models//map3//map3-tree.fbx");

	// polygon scifi asset
	LoadMultiStaticFBX(System, MESH.machine_gun, "Resources//Models//weapon//MG.fbx");
	LoadMultiStaticFBX(System, MESH.dot_machine_gun, "Resources//Models//weapon//dot-MG.fbx");

	// gun flame
	LoadMultiStaticFBX(System, MESH.gun_flame, "Resources//Models//weapon//flame.fbx");
	LoadMultiStaticFBX(System, MESH.gun_flame_back, "Resources//Models//weapon//flame-back.fbx");


	// for test
	LoadAnimatedFBX(System, MESH.gazer, "Resources//TestMesh//Gazer_LP.fbx", "Resources//TestMesh//Gazer_LP.json");
	LoadAnimatedFBX(System, MESH.cop, "Resources//TestMesh//Walk.fbx");
}
/////////////////////////////////////////////////////////////////////////////////

// �ý�ó�� ���⼭ �ε��Ѵ�.
void LoadTexture(DeviceSystem& System) {
	LoadTexture(System, TEX.ColorTex, L"Resources//Image//ColorTexture.png", TEXTURE_TYPE_WIC);

	// map1
	LoadTexture(System, TEX.Palette1, L"Resources//Image//palette-1.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.Palette2, L"Resources//Image//palette-2.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.Palette3, L"Resources//Image//palette-3.png", TEXTURE_TYPE_WIC);

	// map2
	LoadTexture(System, TEX.Map2Palette, L"Resources//Image//GradientSS.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.Map2TerrainTex, L"Resources//Image//map2-terrain.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.IceTex, L"Resources//Image//ice.png", TEXTURE_TYPE_WIC);

	// map3
	LoadTexture(System, TEX.Magma, L"Resources//Image//Cave_Magma_B.png", TEXTURE_TYPE_WIC, D3D12_FILTER_ANISOTROPIC);
	LoadTexture(System, TEX.Map3Palette, L"Resources//Image//Gradients_09.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.Map3RockColor, L"Resources//Image//map3-rock.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.Volcano, L"Resources//Image//volcano.png", TEXTURE_TYPE_WIC);

	// polygon scifi asset
	LoadTexture(System, TEX.scifi, L"Resources//Image//scifi//polygon_scifi.png", TEXTURE_TYPE_WIC);

	// gun flame
	LoadTexture(System, TEX.gun_flame, L"Resources//Image//weapon//flame.png", TEXTURE_TYPE_WIC);
	LoadTexture(System, TEX.gun_flame_back, L"Resources//Image//weapon//flame-back.png", TEXTURE_TYPE_WIC);

	LoadTexture(System, TEX.gazer, L"Resources//TestMesh//Gazer.png", TEXTURE_TYPE_WIC);
}
/////////////////////////////////////////////////////////////////////////////////

Object_Shader* ObjectShader;
Boundbox_Shader* BoundboxShader;
Image_Shader* ImageShader;
Line_Shader* LineShader;

// ������Ʈ ��¿� ���̴� ����
void InitObjectShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	// �Ϲ� ������ ���̴� ����
	ObjectShader = new Object_Shader();
	// �⺻ ���������� ����
	ObjectShader->CreateDefaultPS(Device, RootSignature);
	// ���� �˻� ������ ���������� ����
	ObjectShader->CreateNoneDepthPS(Device, RootSignature);
	// 1��Ī ���������� ����
	ObjectShader->CreateNoneCullingPS(Device, RootSignature);
}

// �̹��� ��¿� ���̴� ����
void InitImageShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	ImageShader = new Image_Shader();
	ImageShader->CreateNoneDepthPS(Device, RootSignature);
}

// �ٿ��ڽ� ���̴� ����
void InitBoundboxShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	BoundboxShader = new Boundbox_Shader();
	BoundboxShader->CreateWireframePS(Device, RootSignature);
	// �ٿ�� ����� ��¿�
	BoundboxShader->CreateDefaultPS(Device, RootSignature);
}

// ���� �귯�� ��¿� ���̴� ����
void InitLineShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	LineShader = new Line_Shader();
	LineShader->CreateNoneDepthPS(Device, RootSignature);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// ���ε� ���۸� ó���ϱ� ���� ����
std::vector<Mesh*> LoadedMeshList;
std::vector<Texture*> LoadedTextureList;

// �⺻ ���� �Ž� �ε�
void LoadSystemMesh(DeviceSystem& System) {
	// �̹��� ��¿� �Ž� ����
	SysRes.ImagePannel = new Mesh;
	SysRes.ImagePannel->CreateImagePannelMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SysRes.ImagePannel);

	// �̹��� �гΰ� ������ �Ž��� ���� ���ؽ��� �����
	SysRes.BillboardMesh = SysRes.ImagePannel;

	// ��ī�̹ڽ� ��¿� �Ž� ����
	SysRes.SkyboxMesh = new Mesh;
	SysRes.SkyboxMesh->CreateSkyboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SysRes.SkyboxMesh);

	// �ٿ��ڽ� ��¿� �Ž� ����
	SysRes.BoundMesh = new Mesh;
	SysRes.BoundMesh->CreateBoundboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SysRes.BoundMesh);

	// �ٿ�彺��� ��¿� �Ž� ����
	SysRes.BoundingSphereMesh = new Mesh(System.Device, System.CmdList, "Resources//SystemResources//Models//BoundingSphereMesh.txt", MESH_TYPE_TEXT);
	LoadedMeshList.emplace_back(SysRes.BoundingSphereMesh);
}

// ���ε� ���۸� �����ϰ�, ���͸� ����.
void ClearUploadBuffer() {
	for (auto const& MeshPtr : LoadedMeshList)
		MeshPtr->ReleaseUploadBuffers();

	for (auto const& TexturePtr : LoadedTextureList)
		TexturePtr->ReleaseUploadBuffers();

	LoadedMeshList.clear();
	LoadedTextureList.clear();
}