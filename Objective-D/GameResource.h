#pragma once
#include "MeshUtil.h"
#include "TextureUtil.h"
#include "ImageShader.h"
#include "ObjectShader.h"
#include "BoundboxShader.h"
#include "LineShader.h"

/////////////////////////////////////////////////////////////////////////////////
// �Ž� ���ҽ��� �ش� Ŭ���� �ȿ� ����
class MeshResource {
public:
	Mesh* GunMesh;
	Mesh* ZombieMesh;
};
extern MeshResource MeshRes;

/////////////////////////////////////////////////////////////////////////////////
// �ؽ�ó ���ҽ��� �ش� Ŭ���� �ȿ� ����
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