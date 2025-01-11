#include "GameResource.h"
// 이 파일은 리소스를 관리한다.
// 기본적으로 전역 리소스이며, ResourceManager.h에 먼저 extern 선언한 뒤, 이 파일에 아래와 같이 정의하면 된다.
// Scene::Init()에서 실행된다.


Mesh* GunMesh;
Mesh* HelicopterMesh;

// 매쉬를 여기서 로드한다.
void LoadMesh(DeviceSystem& System) {
	ImportMesh(System, GunMesh, "Resources//Models//model.bin", MESH_TYPE_BIN);

	// FBX 로드 테스트
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

// 택스처를 여기서 로드한다.
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

// 쉐이더를 여기서 로드한다.
void LoadShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	// 일반 렌더링 쉐이더 생성
	ObjectShader = new Object_Shader();
	// 기본 파이프라인 생성
	ObjectShader->CreateDefaultPS(Device, RootSignature);
	// 깊이 검사 미포함 파이프라인 생성
	ObjectShader->CreateNoneDepthPS(Device, RootSignature);

	// 이미지 출력용 파이프라인 생성
	ImageShader = new Image_Shader();
	ImageShader->CreateNoneDepthPS(Device, RootSignature);

	// 바운드박스 쉐이더 생성
	BoundboxShader = new Boundbox_Shader();
	BoundboxShader->CreateWireframePS(Device, RootSignature);
	BoundboxShader->CreateDefaultPS(Device, RootSignature);

	// 라인 브러쉬 출력용 쉐이더 생성
	LineShader = new Line_Shader();
	LineShader->CreateNoneDepthPS(Device, RootSignature);
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// 업로드 버퍼를 처리하기 위한 벡터
std::vector<Mesh*> LoadedMeshList;
std::vector<Texture*> LoadedTextureList;

Mesh* ImagePannel;
Mesh* BillboardMesh;
Mesh* SkyboxMesh;
Mesh* BoundMesh;
Mesh* BoundingSphereMesh;

// 기본 전역 매쉬 로드
void LoadSystemMesh(DeviceSystem& System) {
	// 이미지 출력용 매쉬 생성
	ImagePannel = new Mesh;
	ImagePannel->CreateImagePannelMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(ImagePannel);

	// 이미지 패널과 빌보드 매쉬는 같은 버텍스를 사용함
	BillboardMesh = ImagePannel;

	// 스카이박스 출력용 매쉬 생성
	SkyboxMesh = new Mesh;
	SkyboxMesh->CreateSkyboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SkyboxMesh);

	// 바운드박스 출력용 매쉬 생성
	BoundMesh = new Mesh;
	BoundMesh->CreateBoundboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(BoundMesh);

	// 바운드스페어 출력용 매쉬 생성
	BoundingSphereMesh = new Mesh(System.Device, System.CmdList, "Resources//SystemResources//Models//BoundingSphereMesh.txt", MESH_TYPE_TEXT);
	LoadedMeshList.emplace_back(BoundingSphereMesh);
}

// 업로드 버퍼를 삭제하고, 벡터를 비운다.
void ClearUploadBuffer() {
	for (auto const& MeshPtr : LoadedMeshList)
		MeshPtr->ReleaseUploadBuffers();

	for (auto const& TexturePtr : LoadedTextureList)
		TexturePtr->ReleaseUploadBuffers();

	LoadedMeshList.clear();
	LoadedTextureList.clear();
}