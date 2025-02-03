#include "GameResource.h"
// �� ������ ���ҽ��� �����Ѵ�.
// �⺻������ ���� ���ҽ��̸�, ResourceManager.h�� ���� extern ������ ��, �� ���Ͽ� �Ʒ��� ���� �����ϸ� �ȴ�.
// Scene::Init()���� ����ȴ�.

MeshResource MeshRes;
TextureResource TexRes;
SystemResource SysRes;


// �Ž��� ���⼭ �ε��Ѵ�.
void LoadMesh(DeviceSystem& System) {
	ImportMesh(System, MeshRes.RockMesh, "Resources//Models//Rock_4.fbx", MESH_TYPE_FBX);
	ImportMesh(System, MeshRes.TestMesh, "Resources//Models//zombie.fbx", MESH_TYPE_FBX);
}
/////////////////////////////////////////////////////////////////////////////////

// �ý�ó�� ���⼭ �ε��Ѵ�.
void LoadTexture(DeviceSystem& System) {
	ImportTexture(System, TexRes.ColorTex, L"Resources//Image//ColorTexture.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.RockTex, L"Resources//Image//texture.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.TestTex, L"Resources//Image//zombie.png", TEXTURE_TYPE_WIC);
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