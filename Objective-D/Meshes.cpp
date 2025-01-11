#include "MeshUtil.h"

// 여기에 매쉬를 작성한다.

// 스카이박스 매쉬를 생성한다. 프로그램 실행 시 최초 1회 실행되고 그 이후에는 굳이 실행할 필요 없다.
void Mesh::CreateSkyboxMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList) {
	float SkyboxVertices[][8] = {
		// x, y, z, nx, ny, nz, u, v
	   // Front face (중앙)
	   { -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.2509f, 0.3339f },  // Top-left
	   {  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.4991f, 0.3339f },  // Top-right
	   {  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.4991f, 0.6661f },  // Bottom-right
	   { -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.2509f, 0.6661f },  // Bottom-left

	   // Back face (오른쪽)
	   {  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.7491f, 0.3339f },  // Top-left
	   { -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.9991f, 0.3339f },  // Top-right
	   { -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.9991f, 0.6661f },  // Bottom-right
	   {  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.7491f, 0.6661f },  // Bottom-left

	   // Left face (왼쪽)
	   { -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0009f, 0.3339f },   // Top-left
	   { -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.2491f, 0.3339f },   // Top-right
	   { -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.2491f, 0.6661f },   // Bottom-right
	   { -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0009f, 0.6661f },   // Bottom-left

	   // Right face (오른쪽 중앙)
	   {  1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.5009f, 0.3339f },    // Top-left
	   {  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.7491f, 0.3339f },    // Top-right
	   {  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.7491f, 0.6661f },    // Bottom-right
	   {  1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.5009f, 0.6661f },    // Bottom-left

	   // Top face (중앙 아래)
	   { -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.2509f, 0.0009f },    // Top-left
	   {  1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.4991f, 0.0009f },    // Top-right
	   {  1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.4991f, 0.3339f },    // Bottom-right
	   { -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.2509f, 0.3339f },    // Bottom-left

	   // Bottom face (중앙 위)
	   { -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.2509f, 0.6661f },   // Top-left
	   {  1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.4991f, 0.6661f },   // Top-right
	   {  1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.4991f, 0.9991f },   // Bottom-right
	   { -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.2509f, 0.9991f }    // Bottom-left
	};

	// 인덱스 배열 (각 면을 두 개의 삼각형으로 구성)
	UINT indices[] = {
		// Front face (시계 방향)
		0, 2, 1, 0, 3, 2,
		// Back face (시계 방향)
		4, 6, 5, 4, 7, 6,
		// Left face (시계 방향)
		8, 10, 9, 8, 11, 10,
		// Right face (시계 방향)
		12, 14, 13, 12, 15, 14,
		// Top face (시계 방향)
		16, 18, 17, 16, 19, 18,
		// Bottom face (시계 방향)
		20, 22, 21, 20, 23, 22
	};


	// 정점 개수와 인덱스 개수 설정
	Vertices = 24;
	Indices = 36;

	// 정점 및 관련 데이터 할당
	Position = new XMFLOAT3[Vertices];
	Normal = new XMFLOAT3[Vertices];
	TextureCoords = new XMFLOAT2[Vertices];
	PnIndices = new UINT[Indices];

	// SkyboxVertices 데이터를 이용하여 각 정점의 포지션, 노말, 텍스처 좌표를 설정
	for (UINT i = 0; i < Vertices; i++) {
		Position[i] = XMFLOAT3(SkyboxVertices[i][0], SkyboxVertices[i][1], SkyboxVertices[i][2]);
		Normal[i] = XMFLOAT3(SkyboxVertices[i][3], SkyboxVertices[i][4], SkyboxVertices[i][5]);
		TextureCoords[i] = XMFLOAT2(SkyboxVertices[i][6], SkyboxVertices[i][7]);
	}

	// 인덱스 데이터 복사
	for (UINT i = 0; i < Indices; i++)
		PnIndices[i] = indices[i];

	// DirectX 버퍼 생성 및 뷰 설정 (원래 코드와 동일)
	PositionBuffer = ::CreateBufferResource(Device, CmdList, Position, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &PositionUploadBuffer);
	NormalBuffer = ::CreateBufferResource(Device, CmdList, Normal, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &NormalUploadBuffer);
	TextureCoordBuffer = ::CreateBufferResource(Device, CmdList, TextureCoords, sizeof(XMFLOAT2) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &TextureCoordUploadBuffer);

	// 버퍼 뷰 설정
	NumVertexBufferViews = 3;
	VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[NumVertexBufferViews];

	VertexBufferViews[0].BufferLocation = PositionBuffer->GetGPUVirtualAddress();
	VertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[1].BufferLocation = NormalBuffer->GetGPUVirtualAddress();
	VertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[2].BufferLocation = TextureCoordBuffer->GetGPUVirtualAddress();
	VertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	VertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * Vertices;

	// 인덱스 버퍼 생성
	IndexBuffer = ::CreateBufferResource(Device, CmdList, PnIndices, sizeof(UINT) * Indices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);

	// 인덱스 버퍼 뷰 설정
	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * Indices;
}

// 이미지 출력을 위한 두께가 없는 정사각형 매쉬를 생성한다. 프로그램 실행 시 최초 1회 실행되고 그 이후에는 굳이 실행할 필요 없다.
void Mesh::CreateImagePannelMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList) {
	float ImagePannel[][8] = {
		// x, y, z, nx, ny, nz, u, v
		{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{  0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		{  0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f }
	};

	// 인덱스 배열
	UINT indices[] = { 0, 2, 1, 0, 3, 2 };

	// 정점 개수와 인덱스 개수
	Vertices = 4;
	Indices = 6;

	// 정점 및 관련 데이터 할당
	Position = new XMFLOAT3[Vertices];
	Normal = new XMFLOAT3[Vertices];
	TextureCoords = new XMFLOAT2[Vertices];
	PnIndices = new UINT[Indices];

	// ImagePannel 데이터를 이용하여 각 정점의 포지션, 노말, 텍스처 좌표를 설정
	for (UINT i = 0; i < Vertices; i++) {
		Position[i] = XMFLOAT3(ImagePannel[i][0], ImagePannel[i][1], ImagePannel[i][2]);
		Normal[i] = XMFLOAT3(ImagePannel[i][3], ImagePannel[i][4], ImagePannel[i][5]);
		TextureCoords[i] = XMFLOAT2(ImagePannel[i][6], ImagePannel[i][7]);
	}

	// 인덱스 데이터 복사
	for (UINT i = 0; i < Indices; i++) {
		PnIndices[i] = indices[i];
	}

	// DirectX 버퍼 생성 (위 코드의 ImportMesh 함수와 유사)
	PositionBuffer = ::CreateBufferResource(Device, CmdList, Position, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &PositionUploadBuffer);
	NormalBuffer = ::CreateBufferResource(Device, CmdList, Normal, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &NormalUploadBuffer);
	TextureCoordBuffer = ::CreateBufferResource(Device, CmdList, TextureCoords, sizeof(XMFLOAT2) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &TextureCoordUploadBuffer);

	// 버퍼 뷰 설정
	NumVertexBufferViews = 3;
	VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[NumVertexBufferViews];

	VertexBufferViews[0].BufferLocation = PositionBuffer->GetGPUVirtualAddress();
	VertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[1].BufferLocation = NormalBuffer->GetGPUVirtualAddress();
	VertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[2].BufferLocation = TextureCoordBuffer->GetGPUVirtualAddress();
	VertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	VertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * Vertices;

	// 인덱스 버퍼 생성
	IndexBuffer = ::CreateBufferResource(Device, CmdList, PnIndices, sizeof(UINT) * Indices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);

	// 인덱스 버퍼 뷰 설정
	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * Indices;
}

// 바운드 박스 출력을 위한 매쉬를 생성한다. 프로그램 실행 시 최초 1회 실행되고 그 이후에는 굳이 실행할 필요 없다.
void Mesh::CreateBoundboxMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList) {
	float BoundboxVertices[][8] = {
		// x, y, z, nx, ny, nz, u, v
		// Front face (중앙)
		{ -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },  // Top-left
		{ -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },  // Bottom-left
		{  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },  // Bottom-right
		{  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },  // Top-right

		// Back face (오른쪽)
		{  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f },  // Top-left
		{  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f },  // Bottom-left
		{ -1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f },  // Bottom-right
		{ -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f },  // Top-right

		// Left face (왼쪽)
		{ -1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },   // Top-left
		{ -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },   // Bottom-left
		{ -1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },   // Bottom-right
		{ -1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },   // Top-right

		// Right face (오른쪽 중앙)
		{  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },    // Top-left
		{  1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },    // Bottom-left
		{  1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },    // Bottom-right
		{  1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },    // Top-right

		// Top face (중앙 아래)
		{ -1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },    // Top-left
		{ -1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },    // Bottom-left
		{  1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },    // Bottom-right
		{  1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },    // Top-right

		// Bottom face (중앙 위)
		{ -1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },   // Top-left
		{ -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },   // Bottom-left
		{  1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },   // Bottom-right
		{  1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f }    // Top-right
	};

	UINT indices[] = {
		// Front face (반시계 방향)
		0, 1, 2, 0, 2, 3,
		// Back face (반시계 방향)
		4, 5, 6, 4, 6, 7,
		// Left face (반시계 방향)
		8, 9, 10, 8, 10, 11,
		// Right face (반시계 방향)
		12, 13, 14, 12, 14, 15,
		// Top face (반시계 방향)
		16, 17, 18, 16, 18, 19,
		// Bottom face (반시계 방향)
		20, 21, 22, 20, 22, 23
	};

	// 정점 개수와 인덱스 개수 설정
	Vertices = 24;
	Indices = 36;

	// 정점 및 관련 데이터 할당
	Position = new XMFLOAT3[Vertices];
	PnIndices = new UINT[Indices];

	// 각 정점의 포지션 좌표를 설정
	for (UINT i = 0; i < Vertices; i++) 
		Position[i] = XMFLOAT3(BoundboxVertices[i][0], BoundboxVertices[i][1], BoundboxVertices[i][2]);
	
	// 인덱스 데이터 복사
	for (UINT i = 0; i < Indices; i++) 
		PnIndices[i] = indices[i];
	
	// DirectX 버퍼 생성 및 뷰 설정 (원래 코드와 동일)
	PositionBuffer = ::CreateBufferResource(Device, CmdList, Position, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &PositionUploadBuffer);

	// 버퍼 뷰 설정
	NumVertexBufferViews = 1;
	VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[NumVertexBufferViews];

	VertexBufferViews[0].BufferLocation = PositionBuffer->GetGPUVirtualAddress();
	VertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	// 인덱스 버퍼 생성
	IndexBuffer = ::CreateBufferResource(Device, CmdList, PnIndices, sizeof(UINT) * Indices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);

	// 인덱스 버퍼 뷰 설정
	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * Indices;
}

// 파일로부터 매쉬를 불러온다,
void Mesh::ImportMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, char* Directory, bool TextMode) {
	char Token[64] = { '\0' };

	if (TextMode) {
		std::ifstream In(Directory);

		while (true) {
			In >> Token;
			if (!In) break;

			if (!strcmp(Token, "<Vertices>:")) {
				In >> Vertices;
				Position = new XMFLOAT3[Vertices];
				for (UINT i = 0; i < Vertices; i++) In >> Position[i].x >> Position[i].y >> Position[i].z;
			}

			else if (!strcmp(Token, "<Normals>:")) {
				In >> Token;
				Normal = new XMFLOAT3[Vertices];
				for (UINT i = 0; i < Vertices; i++) In >> Normal[i].x >> Normal[i].y >> Normal[i].z;
			}

			else if (!strcmp(Token, "<TextureCoords>:")) {
				In >> Token;
				TextureCoords = new XMFLOAT2[Vertices];
				for (UINT i = 0; i < Vertices; i++) In >> TextureCoords[i].x >> TextureCoords[i].y;
			}

			else if (!strcmp(Token, "<Indices>:")) {
				In >> Indices;
				PnIndices = new UINT[Indices];
				for (UINT i = 0; i < Indices; i++) In >> PnIndices[i];
			}
		}
	}

	else {
		FILE* File = NULL;
		::fopen_s(&File, Directory, "rb");
		::rewind(File);

		char Token[64] = { '\0' };

		BYTE StrLength = 0;
		UINT Reads = 0;

		Reads = (UINT)::fread(&StrLength, sizeof(BYTE), 1, File);
		Reads = (UINT)::fread(Token, sizeof(char), 14, File); //"<BoundingBox>:"
		Reads = (UINT)::fread(&OOBB.Center, sizeof(float), 3, File);
		Reads = (UINT)::fread(&OOBB.Extents, sizeof(float), 3, File);

		Reads = (UINT)::fread(&StrLength, sizeof(BYTE), 1, File);
		Reads = (UINT)::fread(Token, sizeof(char), 11, File); //"<Vertices>:"
		Reads = (UINT)::fread(&Vertices, sizeof(int), 1, File);
		Position = new XMFLOAT3[Vertices];
		Reads = (UINT)::fread(Position, sizeof(float), 3 * Vertices, File);

		Reads = (UINT)::fread(&StrLength, sizeof(BYTE), 1, File);
		Reads = (UINT)::fread(Token, sizeof(char), 10, File); //"<Normals>:"
		Reads = (UINT)::fread(&Vertices, sizeof(int), 1, File);
		Normal = new XMFLOAT3[Vertices];
		Reads = (UINT)::fread(Normal, sizeof(float), 3 * Vertices, File);

		Reads = (UINT)::fread(&StrLength, sizeof(BYTE), 1, File);
		Reads = (UINT)::fread(Token, sizeof(char), 16, File); //"<TextureCoords>:"
		Reads = (UINT)::fread(&Vertices, sizeof(int), 1, File);
		TextureCoords = new XMFLOAT2[Vertices];
		Reads = (UINT)::fread(TextureCoords, sizeof(float), 2 * Vertices, File);

		Reads = (UINT)::fread(&StrLength, sizeof(BYTE), 1, File);
		Reads = (UINT)::fread(Token, sizeof(char), 10, File); //"<Indices>:"
		Reads = (UINT)::fread(&Indices, sizeof(int), 1, File);
		PnIndices = new UINT[Indices];
		Reads = (UINT)::fread(PnIndices, sizeof(UINT), Indices, File);

		::fclose(File);
	}

	PositionBuffer = ::CreateBufferResource(Device, CmdList, Position, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &PositionUploadBuffer);
	NormalBuffer = ::CreateBufferResource(Device, CmdList, Normal, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &NormalUploadBuffer);
	TextureCoordBuffer = ::CreateBufferResource(Device, CmdList, TextureCoords, sizeof(XMFLOAT2) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &TextureCoordUploadBuffer);

	NumVertexBufferViews = 3;
	VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[NumVertexBufferViews];

	VertexBufferViews[0].BufferLocation = PositionBuffer->GetGPUVirtualAddress();
	VertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[1].BufferLocation = NormalBuffer->GetGPUVirtualAddress();
	VertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[2].BufferLocation = TextureCoordBuffer->GetGPUVirtualAddress();
	VertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	VertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * Vertices;

	IndexBuffer = ::CreateBufferResource(Device, CmdList, PnIndices, sizeof(UINT) * Indices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * Indices;
}

void Mesh::CreateFBXMesh(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, std::vector<MyVertex>& VertexData) {
	Vertices = static_cast<UINT>(VertexData.size());
	Indices = Vertices;

	Position = new XMFLOAT3[Vertices];
	Normal = new XMFLOAT3[Vertices];
	TextureCoords = new XMFLOAT2[Vertices];
	BoneIndices = new XMUINT4[Vertices];
	BoneWeights = new XMFLOAT4[Vertices];
	PnIndices = new UINT[Indices];

	for (UINT i = 0; i < Vertices; i++) {
		Position[i] = XMFLOAT3(VertexData[i].px, VertexData[i].py, VertexData[i].pz);
		Normal[i] = XMFLOAT3(VertexData[i].nx, VertexData[i].ny, VertexData[i].nz);
		TextureCoords[i] = XMFLOAT2(VertexData[i].u, VertexData[i].v);

		BoneIndices[i] = XMUINT4(VertexData[i].boneIndices[0], VertexData[i].boneIndices[1], VertexData[i].boneIndices[2], VertexData[i].boneIndices[3]);
		BoneWeights[i] = XMFLOAT4(VertexData[i].boneWeights[0], VertexData[i].boneWeights[1], VertexData[i].boneWeights[2], VertexData[i].boneWeights[3]);
	}

	for (UINT i = 0; i < Indices; i++) {
		PnIndices[i] = i;
	}

	PositionBuffer = ::CreateBufferResource(Device, CmdList, Position, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &PositionUploadBuffer);
	NormalBuffer = ::CreateBufferResource(Device, CmdList, Normal, sizeof(XMFLOAT3) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &NormalUploadBuffer);
	TextureCoordBuffer = ::CreateBufferResource(Device, CmdList, TextureCoords, sizeof(XMFLOAT2) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &TextureCoordUploadBuffer);
	BoneIndexBuffer = ::CreateBufferResource(Device, CmdList, BoneIndices, sizeof(XMUINT4) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &BoneIndexUploadBuffer);
	BoneWeightBuffer = ::CreateBufferResource(Device, CmdList, BoneWeights, sizeof(XMFLOAT4) * Vertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &BoneWeightUploadBuffer);

	NumVertexBufferViews = 5;
	VertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[NumVertexBufferViews];

	VertexBufferViews[0].BufferLocation = PositionBuffer->GetGPUVirtualAddress();
	VertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[1].BufferLocation = NormalBuffer->GetGPUVirtualAddress();
	VertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	VertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * Vertices;

	VertexBufferViews[2].BufferLocation = TextureCoordBuffer->GetGPUVirtualAddress();
	VertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	VertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * Vertices;

	VertexBufferViews[3].BufferLocation = BoneIndexBuffer->GetGPUVirtualAddress();
	VertexBufferViews[3].StrideInBytes = sizeof(XMUINT4);
	VertexBufferViews[3].SizeInBytes = sizeof(XMUINT4) * Vertices;

	VertexBufferViews[4].BufferLocation = BoneWeightBuffer->GetGPUVirtualAddress();
	VertexBufferViews[4].StrideInBytes = sizeof(XMFLOAT4);
	VertexBufferViews[4].SizeInBytes = sizeof(XMFLOAT4) * Vertices;

	IndexBuffer = ::CreateBufferResource(Device, CmdList, PnIndices, sizeof(UINT) * Indices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &IndexUploadBuffer);
	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = sizeof(UINT) * Indices;

	std::cout << "FBX Mesh with Skinning created" << std::endl;
}