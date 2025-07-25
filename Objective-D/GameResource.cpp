#include "GameResource.h"
#include <string>
// 이 파일은 리소스를 관리한다.
// 기본적으로 전역 리소스이며, ResourceManager.h에 먼저 extern 선언한 뒤, 이 파일에 아래와 같이 정의하면 된다.
// Scene::Init()에서 실행된다.

MeshResource MESH;
TextureResource TEX;
SystemResource SYSRES;
GlobalValue GLOBAL;

// 전역 카메라 fov 값 오프셋
float globalFovOffset;

bool player_enter;
unsigned int enter_player_id;

DeviceSystem LoadSystem;

// 개발 시 로드 시간 단축을 위해 선택적으로 리소스를 로드할 수 있도록 하였다.
// DevMode 활성화 시에만 아래 3개의 플래그가 의미가 있음
bool DevMode = true;

bool LoadMap1Resources = true;
bool LoadMap2Resources = false;
bool LoadMap3Resources = false;

// 매쉬를 여기서 로드한다.
void LoadMesh(DeviceSystem& System) {
	LoadSystem = System;

	// DevMode가 아닐 경우 모두 로드하도록 강제한다.
	if (!DevMode) {
		LoadMap1Resources = true;
		LoadMap2Resources = true;
		LoadMap3Resources = true;
	}

	// map1
	if (LoadMap1Resources) {
		LoadSingleStaticFBX(MESH.RockMesh, "Resources//Models//map1//wall-rock.fbx");
		LoadSingleStaticFBX(MESH.LakeMesh, "Resources//Models//map1//lake.fbx");

		for (int i = 0; i < 3; ++i) {
			std::string FileName = "Resources//Models//map1//lake-rock-";
			FileName += std::to_string(i + 1) + ".fbx";
			LoadSingleStaticFBX(MESH.LakeRockMesh[i], (char*)FileName.c_str());
		}

		for (int i = 0; i < 5; ++i) {
			std::string FileName = "Resources//Models//map1//map-object-";
			FileName += std::to_string(i + 1) + ".fbx";
			LoadSingleStaticFBX(MESH.MapObjectMesh[i], (char*)FileName.c_str());
		}

		LoadSingleStaticFBX(MESH.Grass[0], "Resources//Models//map1//grass-1.fbx");
		LoadSingleStaticFBX(MESH.Grass[1], "Resources//Models//map1//grass-2.fbx");
		LoadSingleStaticFBX(MESH.Flower[0], "Resources//Models//map1//flower-1.fbx");
		LoadSingleStaticFBX(MESH.Flower[1], "Resources//Models//map1//flower-2.fbx");
		LoadSingleStaticFBX(MESH.Mushroom[0], "Resources//Models//map1//mushroom-1.fbx");
		LoadSingleStaticFBX(MESH.Mushroom[1], "Resources//Models//map1//mushroom-2.fbx");

		// monster - scorpion
		LoadAnimatedFBX(MESH.scorpion, "Resources//Models//monster//scorpion.fbx",
			"Resources//Animation Data//scorpion.pca", "Resources//Models//monster//scorpion.json", false);

		// monster - plant monster
		LoadAnimatedFBX(MESH.plantMonster,
			"Resources//Models//monster//plant_monster.fbx",
			"Resources//Animation Data///plant_monster.pca", "Resources//Models//monster//plant_monster.json", false);
		SetAnimationOffset(MESH.plantMonster, 1.6666666269302369);
	}

	// map2
	if (LoadMap2Resources) {
		LoadSingleStaticFBX(MESH.WinterWall, "Resources//Models//map2//winter-cliff.fbx");

		for (int i = 0; i < 3; ++i) {
			std::string FileName = "Resources//Models//map2//winter-rock-" + std::to_string(i + 1) + ".fbx";
			LoadSingleStaticFBX(MESH.WinterRock[i], (char*)FileName.c_str());
		}

		for (int i = 0; i < 2; ++i) {
			std::string FileName = "Resources//Models//map2//winter-ice-" + std::to_string(i + 1) + ".fbx";
			LoadSingleStaticFBX(MESH.WinterIce[i], (char*)FileName.c_str());
		}

		// monster - troll
		LoadAnimatedFBX(MESH.troll,
			"Resources//Models//monster//troll.fbx",
			"Resources//Animation Data///troll.pca", "Resources//Models//monster//troll.json", false);
		SetAnimationOffset(MESH.troll, 0.4333333373069763);

		// monster - treant
		LoadAnimatedFBX(MESH.treant[0], "Resources//Models//monster//treantIdle.fbx", "Resources//Animation Data//treantIdle.pca", "", false);
		LoadAnimatedFBX(MESH.treant[1], "Resources//Models//monster//treantMove.fbx", "Resources//Animation Data//treantMove.pca", "", false);
		LoadAnimatedFBX(MESH.treant[2], "Resources//Models//monster//treantAttack.fbx", "Resources//Animation Data//treantAttack.pca", "", false);
		LoadAnimatedFBX(MESH.treant[3], "Resources//Models//monster//treantDeath.fbx", "Resources//Animation Data//treantDeath.pca", "", false);
	}

	// 맵 1, 2와 공유하므로 둘 다 로드 안할 시에만 로드 안 함
	if(LoadMap2Resources || LoadMap1Resources)
		LoadSingleStaticFBX(MESH.TerrainMesh1, "Resources//Models//map1//terrain.fbx");

	// map3
	if(LoadMap3Resources) {
		LoadSingleStaticFBX(MESH.FloatingRock, "Resources//Models//map3//map3-rock.fbx");
		LoadSingleStaticFBX(MESH.Volcano, "Resources//Models//map3//volcano.fbx");
		LoadSingleStaticFBX(MESH.SmallVolcano, "Resources//Models//map3//volcano-small.fbx");

		for (int i = 0; i < 2; i++) {
			std::string FileName = "Resources//Models//map3//map3-stone-" + std::to_string(i + 1) + ".fbx";
			LoadSingleStaticFBX(MESH.Map3Stone[i], (char*)FileName.c_str());
		}

		for (int i = 0; i < 3; ++i) {
			std::string FileName = "Resources//Models//map3//map3-crystal-" + std::to_string(i + 1) + ".fbx";
			LoadSingleStaticFBX(MESH.Crystal[i], (char*)FileName.c_str());
		}

		LoadSingleStaticFBX(MESH.DeadTree, "Resources//Models//map3//map3-tree.fbx");

		// monster - gazer
		LoadAnimatedFBX(MESH.gazer, "Resources//Models//monster//gazer.fbx", "Resources//Animation Data//gazer.pca",
			"Resources//Models//monster//gazer.json", true);

		// monster - imp
		LoadAnimatedFBX(MESH.imp, "Resources//Models//monster//imp.fbx", "Resources//Animation Data//imp.pca",
			"Resources//Models//monster//imp.json", true);
	}

	// 항상 필요한 리소스들은 여기에서 로드한다.
	// explosion effect
	LoadAnimatedFBX(MESH.explosion, "Resources//Models//effect//explosion.fbx",
		"Resources//Animation Data//explosion.pca", "", false);
	
	// polygon scifi asset
	LoadMultiStaticFBX(MESH.center_building, "Resources//Models//building//center.fbx");
	LoadMultiStaticFBX(MESH.grenade, "Resources//Models//weapon//grenade.fbx");

	LoadMultiStaticFBX(MESH.machine_gun, "Resources//Models//weapon//MG.fbx");
	LoadMultiStaticFBX(MESH.dot_machine_gun, "Resources//Models//weapon//dot-MG.fbx");

	LoadMultiStaticFBX(MESH.shotgun, "Resources//Models//weapon//shotgun.fbx");
	LoadMultiStaticFBX(MESH.dot_shotgun, "Resources//Models//weapon//dot-shotgun.fbx");

	// turret asset
	LoadMultiStaticFBX(MESH.turretHead, "Resources//Models//weapon//turretHead.fbx");
	LoadMultiStaticFBX(MESH.turretTorso, "Resources//Models//weapon//turretTorso.fbx");
	LoadMultiStaticFBX(MESH.turretBottom, "Resources//Models//weapon//turretBottom.fbx");

	// gun flame
	LoadMultiStaticFBX(MESH.gun_flame, "Resources//Models//weapon//flame.fbx");
	LoadMultiStaticFBX(MESH.gun_flame_back, "Resources//Models//weapon//flame-back.fbx");

	/*LoadMultiStaticFBX(MESH.turretFlame, "Resources//Models//weapon//flame-turret.fbx");
	LoadMultiStaticFBX(MESH.turretFlameBack, "Resources//Models//weapon//flame-turret-back.fbx");*/

	// polygon scifi player asset - heavy
	LoadAnimatedFBX(MESH.heavyIdle, "Resources//Models//player//heavy//heavy_idle.fbx", 
		"Resources//Animation Data//heavy_idle.pca", "", false);

	LoadAnimatedFBX(MESH.heavyMove, "Resources//Models//player//heavy//heavy_move.fbx", 
		"Resources//Animation Data///heavy_move.pca", "", false);

	LoadAnimatedFBX(MESH.heavyShoot, "Resources//Models//player//heavy//heavy_shoot.fbx", 
		"Resources//Animation Data///heavy_shoot.pca", "", false);

	LoadAnimatedFBX(MESH.heavyDeath, "Resources//Models//player//heavy//heavy_death.fbx", 
		"Resources//Animation Data///heavy_death.pca", "", false);
}
/////////////////////////////////////////////////////////////////////////////////

// 택스처를 여기서 로드한다.
void LoadTexture(DeviceSystem& System) {
	LoadSystem = System;

	LoadTexture(TEX.ColorTex, L"Resources//Image//ColorTexture.png", TEXTURE_TYPE_WIC);

	// fontAtlas
	int CropSize = 2048 / 16;      // 각 셀의 가로/세로 크기
	int Columns = 16;      // 한 줄당 셀 수
	int Offset = 2048 / 16 * 2;

	for (int i = 0; i < 96; ++i) {
		int x = (i % Columns) * CropSize;
		int y = Offset + (i / Columns) * CropSize;
		LoadCropTexture(TEX.fontAtlas[i], L"Resources//Image//ui//font_atlas.png", x, y, CropSize, CropSize);
	}

	int smokeCropSize = 960 / 5;
	int smokeColums = 5;
	int smokeOffset = 0;

	for (int i = 0; i < 25; ++i) {
		int x = (i % smokeColums) * smokeCropSize;
		int y = smokeOffset + (i / smokeColums) * smokeCropSize;
		LoadCropTexture(TEX.smoke[i], L"Resources//Image//smoke.png", x, y, smokeCropSize, smokeCropSize);
	}

	LoadTexture(TEX.ColorTex, L"Resources//Image//ColorTexture.png", TEXTURE_TYPE_WIC);
	LoadTexture(TEX.skyBox, L"Resources//Image//sky.png", TEXTURE_TYPE_WIC);
	LoadTexture(TEX.skyBox2, L"Resources//Image//sky-2.png", TEXTURE_TYPE_WIC);

	// map1
	if (LoadMap1Resources) {
		LoadTexture(TEX.Palette1, L"Resources//Image//palette-1.png", TEXTURE_TYPE_WIC);
		LoadTexture(TEX.Palette2, L"Resources//Image//palette-2.png", TEXTURE_TYPE_WIC);
	}

	// map2
	if (LoadMap2Resources) {
		LoadTexture(TEX.Map2Palette, L"Resources//Image//GradientSS.png", TEXTURE_TYPE_WIC);
		LoadTexture(TEX.Map2TerrainTex, L"Resources//Image//map2-terrain.png", TEXTURE_TYPE_WIC);
		LoadTexture(TEX.IceTex, L"Resources//Image//ice.png", TEXTURE_TYPE_WIC);
	}

	if (LoadMap2Resources || LoadMap1Resources) {
		LoadTexture(TEX.Palette3, L"Resources//Image//palette-3.png", TEXTURE_TYPE_WIC);
		// sky box
	}

	// map3
	if (LoadMap3Resources) {
		LoadTexture(TEX.Magma, L"Resources//Image//Cave_Magma_B.png", TEXTURE_TYPE_WIC, D3D12_FILTER_ANISOTROPIC);
		LoadTexture(TEX.Map3Palette, L"Resources//Image//Gradients_09.png", TEXTURE_TYPE_WIC);
		LoadTexture(TEX.Map3RockColor, L"Resources//Image//map3-rock.png", TEXTURE_TYPE_WIC);
		LoadTexture(TEX.Volcano, L"Resources//Image//volcano.png", TEXTURE_TYPE_WIC);
	}

	// roadblock
	LoadTexture(TEX.roadBlock, L"Resources//Image//roadblock.png");

	// polygon scifi asset
	LoadTexture(TEX.scifi, L"Resources//Image//scifi//polygon_scifi.png", TEXTURE_TYPE_WIC);

	// gun flame
	LoadTexture(TEX.gun_flame, L"Resources//Image//weapon//flame.png", TEXTURE_TYPE_WIC);
	LoadTexture(TEX.gun_flame_back, L"Resources//Image//weapon//flame-back.png", TEXTURE_TYPE_WIC);
	LoadTexture(TEX.muzzle_particle, L"Resources//Image//weapon//muzzle_particle.png", TEXTURE_TYPE_WIC);

	// turret
	LoadTexture(TEX.turret, L"Resources//Image//turret.png");

	LoadTexture(TEX.poisonBall, L"Resources//Image//plant_monster_bullet.png");

	// map1 monster
	LoadTexture(TEX.plantMonster, L"Resources//Image//monster//plant_monster.png");
	LoadTexture(TEX.scorpion, L"Resources//Image//monster//scorpion.png");

	// map2 monster
	LoadTexture(TEX.troll, L"Resources//Image//monster//troll.png");
	LoadTexture(TEX.treant, L"Resources//Image//monster//treant.png");

	// map3 monster
	LoadTexture(TEX.gazer, L"Resources//Image//monster//gazer.png");
	LoadTexture(TEX.imp, L"Resources//Image//monster//imp.png");

	// UI
	LoadTexture(TEX.UI_map1Enemy, L"Resources//Image//ui//map1_enemy.png");
	LoadTexture(TEX.UI_playerHit, L"Resources//Image//ui//playerHit.png");
}
/////////////////////////////////////////////////////////////////////////////////

Object_Shader* ObjectShader;
Boundbox_Shader* BoundboxShader;
Image_Shader* ImageShader;
Line_Shader* LineShader;

// 오브젝트 출력용 쉐이더 생성
void InitObjectShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	// 일반 렌더링 쉐이더 생성
	ObjectShader = new Object_Shader();
	// 기본 파이프라인 생성
	ObjectShader->CreateDefaultPS(Device, RootSignature);
	// 깊이 쓰기 미포함 파이프라인 샌성
	ObjectShader->CreateTransparentDefaultPS(Device, RootSignature);
	// 깊이 검사 미포함 파이프라인 생성
	ObjectShader->CreateNoneDepthPS(Device, RootSignature);
	// 1인칭 파이프라인 생성
	ObjectShader->CreateNoneCullingPS(Device, RootSignature);
}

// 이미지 출력용 쉐이더 생성
void InitImageShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	ImageShader = new Image_Shader();
	ImageShader->CreateNoneDepthPS(Device, RootSignature);
}

// 바운드박스 쉐이더 생성
void InitBoundboxShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	BoundboxShader = new Boundbox_Shader();
	BoundboxShader->CreateWireframePS(Device, RootSignature);
	// 바운드 스페어 출력용
	BoundboxShader->CreateDefaultPS(Device, RootSignature);
}

// 라인 브러쉬 출력용 쉐이더 생성
void InitLineShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device) {
	LineShader = new Line_Shader();
	LineShader->CreateNoneDepthPS(Device, RootSignature);
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// 업로드 버퍼를 처리하기 위한 벡터
std::vector<Mesh*> LoadedMeshList;
std::vector<Texture*> LoadedTextureList;

// 기본 전역 매쉬 로드
void LoadSystemMesh(DeviceSystem& System) {
	// 이미지 출력용 매쉬 생성
	SYSRES.ImagePannel = new Mesh;
	SYSRES.ImagePannel->CreateImagePannelMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SYSRES.ImagePannel);

	// 이미지 패널과 빌보드 매쉬는 같은 버텍스를 사용함
	SYSRES.BillboardMesh = SYSRES.ImagePannel;

	// 스카이박스 출력용 매쉬 생성
	SYSRES.SkyboxMesh = new Mesh;
	SYSRES.SkyboxMesh->CreateSkyboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SYSRES.SkyboxMesh);

	// 바운드박스 출력용 매쉬 생성
	SYSRES.BoundMesh = new Mesh;
	SYSRES.BoundMesh->CreateBoundboxMesh(System.Device, System.CmdList);
	LoadedMeshList.emplace_back(SYSRES.BoundMesh);

	// 바운드스페어 출력용 매쉬 생성
	SYSRES.BoundingSphereMesh = new Mesh(System.Device, System.CmdList, "Resources//SystemResources//Models//BoundingSphereMesh.txt", MESH_TYPE_TEXT);
	LoadedMeshList.emplace_back(SYSRES.BoundingSphereMesh);
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