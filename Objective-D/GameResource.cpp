#include "GameResource.h"
// �� ������ ���ҽ��� �����Ѵ�.
// �⺻������ ���� ���ҽ��̸�, ResourceManager.h�� ���� extern ������ ��, �� ���Ͽ� �Ʒ��� ���� �����ϸ� �ȴ�.
// Scene::Init()���� ����ȴ�.


Mesh* GunMesh;
Mesh* HelicopterMesh;

// �Ž��� ���⼭ �ε��Ѵ�.
void LoadMesh(DeviceSystem& System) {
	ImportMesh(System, GunMesh, "Resources//Models//model.bin", MESH_TYPE_BIN);

	// FBX �ε� �׽�Ʈ
	fbxUtil.InitializeFBX(fbxUtil.manager, fbxUtil.scene);
	if (fbxUtil.LoadFBXFile(fbxUtil.manager, fbxUtil.scene, "Resources//Models//zombie.fbx")) {
		fbxUtil.TriangulateScene(fbxUtil.manager, fbxUtil.scene);
		fbxUtil.GetVertexData(fbxUtil.scene, parsedVertices);

		//fbxUtil.ProcessAnimation(fbxUtil.scene, animations);
		//fbxUtil.PrintAnimationData(animations);
		HelicopterMesh = new Mesh();
		HelicopterMesh->CreateFBXMesh(System.Device, System.CmdList, parsedVertices);
	}
}
/////////////////////////////////////////////////////////////////////////////////


Texture* Tex, * SkyboxTex, * WoodTex;
Texture* ColorTex;
Texture* HelicopterTex;

// �ý�ó�� ���⼭ �ε��Ѵ�.
void LoadTexture(DeviceSystem& System) {
	ImportTexture(System, Tex, L"Resources//Image//Gun.jpg", TEXTURE_TYPE_WIC);
	ImportTexture(System, WoodTex, L"Resources//Image//Wood.jpg", TEXTURE_TYPE_WIC);
	ImportTexture(System, ColorTex, L"Resources//Image//ColorTexture.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, HelicopterTex, L"Resources//Image//zombie.png", TEXTURE_TYPE_WIC, D3D12_FILTER_ANISOTROPIC);
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

Mesh* ImagePannel;
Mesh* BillboardMesh;
Mesh* SkyboxMesh;
Mesh* BoundMesh;
Mesh* BoundingSphereMesh;

// �⺻ ���� �Ž� �ε�
void LoadSystemMesh(DeviceSystem& System) {
	// �̹��� ��¿� �Ž� ����
	ImagePannel = new Mesh;
	ImagePannel->CreateImagePannelMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(ImagePannel);

	// �̹��� �гΰ� ������ �Ž��� ���� ���ؽ��� �����
	BillboardMesh = ImagePannel;

	// ��ī�̹ڽ� ��¿� �Ž� ����
	SkyboxMesh = new Mesh;
	SkyboxMesh->CreateSkyboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SkyboxMesh);

	// �ٿ��ڽ� ��¿� �Ž� ����
	BoundMesh = new Mesh;
	BoundMesh->CreateBoundboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(BoundMesh);

	// �ٿ�彺��� ��¿� �Ž� ����
	BoundingSphereMesh = new Mesh(System.Device, System.CmdList, "Resources//SystemResources//Models//BoundingSphereMesh.txt", MESH_TYPE_TEXT);
	LoadedMeshList.emplace_back(BoundingSphereMesh);
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