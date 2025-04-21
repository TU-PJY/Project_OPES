#include "GameResource.h"
#include <string>
// �� ������ ���ҽ��� �����Ѵ�.
// �⺻������ ���� ���ҽ��̸�, ResourceManager.h�� ���� extern ������ ��, �� ���Ͽ� �Ʒ��� ���� �����ϸ� �ȴ�.
// Scene::Init()���� ����ȴ�.

MeshResource MeshRes;
TextureResource TexRes;
SystemResource SysRes;


// �Ž��� ���⼭ �ε��Ѵ�.
void LoadMesh(DeviceSystem& System) {
	//ImportFBX(System, MeshRes.AMesh, "Resources//TestMesh//test.fbx");
	ImportFBX(System, MeshRes.Lain, "Resources//TestMesh//lain.fbx");

	// �ϴ� FBX �ִϸ��̼� ������ ���� �׽�Ʈ�� �ʿ���� �͵��� �ε����� �ʰ� �Ͻ������� ��Ȱ��ȭ �Ѵ�.
	/*
	ImportMesh(System, MeshRes.TestMesh, "Resources//Models//zombie.fbx", MESH_TYPE_FBX);

	// map1
	ImportMesh(System, MeshRes.RockMesh, "Resources//Models//map1//wall-rock.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.LakeMesh, "Resources//Models//map1//lake.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.TerrainMesh1, "Resources//Models//map1//terrain.fbx", MESH_TYPE_FBX);

	for (int i = 0; i < 3; ++i) {
		std::string FileName = "Resources//Models//map1//lake-rock-";
		FileName += std::to_string(i + 1) + ".fbx";
		ImportMesh(System, MeshRes.LakeRockMesh[i], (char*)FileName.c_str(), MESH_TYPE_FBX);
	}

	for (int i = 0; i < 5; ++i) {
		std::string FileName = "Resources//Models//map1//map-object-";
		FileName += std::to_string(i + 1) + ".fbx";
		ImportMesh(System, MeshRes.MapObjectMesh[i], (char*)FileName.c_str(), MESH_TYPE_FBX);
	}

	ImportMesh(System, MeshRes.Grass[0], "Resources//Models//map1//grass-1.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.Grass[1], "Resources//Models//map1//grass-2.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.Flower[0], "Resources//Models//map1//flower-1.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.Flower[1], "Resources//Models//map1//flower-2.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.Mushroom[0], "Resources//Models//map1//mushroom-1.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.Mushroom[1], "Resources//Models//map1//mushroom-2.fbx", MESH_TYPE_FBX);


	// map2
	ImportMesh(System, MeshRes.WinterWall, "Resources//Models//map2//winter-cliff.fbx", MESH_TYPE_FBX);

	for (int i = 0; i < 3; ++i) {
		std::string FileName = "Resources//Models//map2//winter-rock-" + std::to_string(i + 1) + ".fbx";
		ImportMesh(System, MeshRes.WinterRock[i], (char*)FileName.c_str(), MESH_TYPE_FBX);
	}

	for (int i = 0; i < 2; ++i) {
		std::string FileName = "Resources//Models//map2//winter-ice-" + std::to_string(i + 1) + ".fbx";
		ImportMesh(System, MeshRes.WinterIce[i], (char*)FileName.c_str(), MESH_TYPE_FBX);
	}

	// map3
	ImportMesh(System, MeshRes.FloatingRock, "Resources//Models//map3//map3-rock.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.Volcano, "Resources//Models//map3//volcano.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.SmallVolcano, "Resources//Models//map3//volcano-small.fbx", MESH_TYPE_FBX);

	for (int i = 0; i < 2; i++) {
		std::string FileName = "Resources//Models//map3//map3-stone-" + std::to_string(i + 1) + ".fbx";
		ImportMesh(System, MeshRes.Map3Stone[i], (char*)FileName.c_str(), MESH_TYPE_FBX);
	}

	for (int i = 0; i < 3; ++i) {
		std::string FileName = "Resources//Models//map3//map3-crystal-" + std::to_string(i + 1) + ".fbx";
		ImportMesh(System, MeshRes.Crystal[i], (char*)FileName.c_str(), MESH_TYPE_FBX);
	}

	ImportMesh(System, MeshRes.DeadTree, "Resources//Models//map3//map3-tree.fbx", MESH_TYPE_FBX);
	*/
}
/////////////////////////////////////////////////////////////////////////////////

// �ý�ó�� ���⼭ �ε��Ѵ�.
void LoadTexture(DeviceSystem& System) {
	// FBX �׽�Ʈ�� �� �ؽ��� �ε�
	//ImportTexture(System, TexRes.Man, L"Resources//TestMesh//test.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Lain, L"Resources//TestMesh//lain.png", TEXTURE_TYPE_WIC);

	// FBX �ִϸ��̼� �׽�Ʈ�� �ʿ���� �͵��� �Ͻ������� ��Ȱ��ȭ �Ѵ�.
	/*
	ImportTexture(System, TexRes.ColorTex, L"Resources//Image//ColorTexture.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.TestTex, L"Resources//Image//zombie.png", TEXTURE_TYPE_WIC);

	// map1
	ImportTexture(System, TexRes.Palette1, L"Resources//Image//palette-1.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Palette2, L"Resources//Image//palette-2.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Palette3, L"Resources//Image//palette-3.png", TEXTURE_TYPE_WIC);

	// map2
	ImportTexture(System, TexRes.Map2Palette, L"Resources//Image//GradientSS.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Map2TerrainTex, L"Resources//Image//map2-terrain.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.IceTex, L"Resources//Image//ice.png", TEXTURE_TYPE_WIC);

	// map3
	ImportTexture(System, TexRes.Magma, L"Resources//Image//Cave_Magma_B.png", TEXTURE_TYPE_WIC, D3D12_FILTER_ANISOTROPIC);
	ImportTexture(System, TexRes.Map3Palette, L"Resources//Image//Gradients_09.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Map3RockColor, L"Resources//Image//map3-rock.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Volcano, L"Resources//Image//volcano.png", TEXTURE_TYPE_WIC);
	*/
}
/////////////////////////////////////////////////////////////////////////////////

Object_Shader* ObjectShader;
Boundbox_Shader* BoundboxShader;
Image_Shader* ImageShader;
Line_Shader* LineShader;
// ���̴��� ���⼭ �ε��Ѵ�.
void LoadShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	// �Ϲ� ������ ���̴� ����
	ObjectShader = new Object_Shader();
	// �⺻ ���������� ����
	ObjectShader->CreateDefaultPS(Device, RootSignature);
	// ���� �˻� ������ ���������� ����
	ObjectShader->CreateNoneDepthPS(Device, RootSignature);

	// �̹��� ��¿� ���������� ����
	ImageShader = new Image_Shader();
	ImageShader->CreateNoneDepthPS(Device, RootSignature);

	// �ٿ��ڽ� ���̴� ����
	BoundboxShader = new Boundbox_Shader();
	BoundboxShader->CreateWireframePS(Device, RootSignature);
	BoundboxShader->CreateDefaultPS(Device, RootSignature);

	// ���� �귯�� ��¿� ���̴� ����
	LineShader = new Line_Shader();
	LineShader->CreateNoneDepthPS(Device, RootSignature);
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// ���ε� ���۸� ó���ϱ� ���� ����
std::vector<Mesh*> LoadedMeshList;
std::vector<Texture*> LoadedTextureList;
std::vector<FBXMesh> LoadedFBXMeshList;

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

	for (auto const& FBXPtr : LoadedFBXMeshList) {
		for (auto const& M : FBXPtr.MeshPart)
			M->ReleaseUploadBuffers();
	}

	LoadedMeshList.clear();
	LoadedTextureList.clear();
	LoadedFBXMeshList.clear();
}