#include "GameResource.h"
#include <string>
// 이 파일은 리소스를 관리한다.
// 기본적으로 전역 리소스이며, ResourceManager.h에 먼저 extern 선언한 뒤, 이 파일에 아래와 같이 정의하면 된다.
// Scene::Init()에서 실행된다.

MeshResource MeshRes;
TextureResource TexRes;
SystemResource SysRes;


// 매쉬를 여기서 로드한다.
void LoadMesh(DeviceSystem& System) {
	//ImportFBX(System, MeshRes.AMesh, "Resources//TestMesh//test.fbx");
	ImportFBX(System, MeshRes.Lain, "Resources//TestMesh//lain.fbx");

	// 일단 FBX 애니메이션 구현을 위해 테스트에 필요없는 것들을 로드하지 않고 일시적으로 비활성화 한다.
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

// 택스처를 여기서 로드한다.
void LoadTexture(DeviceSystem& System) {
	// FBX 테스트용 모델 텍스터 로드
	//ImportTexture(System, TexRes.Man, L"Resources//TestMesh//test.png", TEXTURE_TYPE_WIC);
	ImportTexture(System, TexRes.Lain, L"Resources//TestMesh//lain.png", TEXTURE_TYPE_WIC);

	// FBX 애니메이션 테스트에 필요없는 것들을 일시적으로 비활성화 한다.
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
std::vector<FBXMesh> LoadedFBXMeshList;

// 기본 전역 매쉬 로드
void LoadSystemMesh(DeviceSystem& System) {
	// 이미지 출력용 매쉬 생성
	SysRes.ImagePannel = new Mesh;
	SysRes.ImagePannel->CreateImagePannelMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SysRes.ImagePannel);

	// 이미지 패널과 빌보드 매쉬는 같은 버텍스를 사용함
	SysRes.BillboardMesh = SysRes.ImagePannel;

	// 스카이박스 출력용 매쉬 생성
	SysRes.SkyboxMesh = new Mesh;
	SysRes.SkyboxMesh->CreateSkyboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SysRes.SkyboxMesh);

	// 바운드박스 출력용 매쉬 생성
	SysRes.BoundMesh = new Mesh;
	SysRes.BoundMesh->CreateBoundboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SysRes.BoundMesh);

	// 바운드스페어 출력용 매쉬 생성
	SysRes.BoundingSphereMesh = new Mesh(System.Device, System.CmdList, "Resources//SystemResources//Models//BoundingSphereMesh.txt", MESH_TYPE_TEXT);
	LoadedMeshList.emplace_back(SysRes.BoundingSphereMesh);
}

// 업로드 버퍼를 삭제하고, 벡터를 비운다.
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