#pragma once
#include "MeshUtil.h"
#include "TextureUtil.h"
#include "ImageShader.h"
#include "ObjectShader.h"
#include "BoundboxShader.h"
#include "LineShader.h"

/////////////////////////////////////////////////////////////////////////////////
// 매쉬 리소스는 해당 클래스 안에 선언
class MeshResource {
public:
	Mesh* GunMesh;
	Mesh* ZombieMesh;
};
extern MeshResource MeshRes;

/////////////////////////////////////////////////////////////////////////////////
// 텍스처 리소스는 해당 클래스 안에 선언
class TextureResource {
public:
	Texture* Tex, * SkyboxTex, * WoodTex;
	Texture* ColorTex;
	Texture* ZombieTex;
};
extern TextureResource TexRes;

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
extern SystemResource SysRes;

void LoadShader(ID3D12RootSignature* RootSignature, ID3D12Device* Device);
void LoadSystemMesh(DeviceSystem& System);
void LoadMesh(DeviceSystem& System);
void LoadTexture(DeviceSystem& System);
void ClearUploadBuffer();

inline void ImportMesh(DeviceSystem& System, Mesh*& MeshPtr, char* Directory, int Type) {
	if (Type != MESH_TYPE_FBX)
		MeshPtr = new Mesh(System.Device, System.CmdList, Directory, Type);

	else {
		if (fbxUtil.LoadFBXFile(Directory)) {
			fbxUtil.TriangulateScene();
			fbxUtil.GetVertexData();
			MeshPtr = new Mesh();
			MeshPtr->CreateFBXMesh(System.Device, System.CmdList, fbxUtil.GetVertexVector());
		}
	}
	LoadedMeshList.emplace_back(MeshPtr);
}

inline void ImportTexture(DeviceSystem& System, Texture*& TexturePtr, wchar_t* Directory, int Type, D3D12_FILTER FilterOption=D3D12_FILTER_MIN_MAG_MIP_LINEAR) {
	TexturePtr = new Texture(System.Device, System.CmdList, Directory, Type, FilterOption);
	LoadedTextureList.emplace_back(TexturePtr);
}