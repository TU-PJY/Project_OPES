#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#define MAX_LOADSTRING 100
#include "Config.h"
#include "Objective-D.h"
#include "Framework.h"

#include "Scene.h"
#include "CameraUtil.h"
#include "ShaderUtil.h"
#include "MouseUtil.h"
#include "TerrainUtil.h"

#include <locale>

#include <string>

//서버
#include <winsock2.h>
//#include <windows.h>
#include <Ws2tcpip.h>  
#include <atlconv.h>
//#include <iostream>
//#include <conio.h>
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include"../Server_OPES/Packet.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "msimg32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000

SOCKET clientSocket;
bool isRunning = true;
bool enter_room = true;//false;
WSABUF recv_wsabuf[1];
char recv_buffer[MAX_SOCKBUF];
WSAOVERLAPPED recv_over;
bool useServer = true;//클라만 켜서 할땐 false로 바꿔서하기

std::set<unsigned int> ID_List;

class OtherPlayer : public GameObject {
public:
	OtherPlayer() {
		//SelectFBXAnimation(MESH.gazer, "Idle");
	}

	XMFLOAT3 position{};
	XMFLOAT3 rotation{};
	Vector vec{};

	void InputPosition(XMFLOAT3& value) override {
		position = value;
	}

	void InputRotation(XMFLOAT3& value) override {
		rotation = value;
	}

	void Update(float FrameTime) {
		UpdateFBXAnimation(MESH.cop, FrameTime);
	}

	void Render() {
		BeginRender();
		Transform::Move(TranslateMatrix, position);
		Transform::Rotate(RotateMatrix, 0.0, rotation.y, 0.0);
		RenderFBX(MESH.cop, TEX.scifi);
	}
};


void CALLBACK RecvCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED p_over, DWORD flag) {
	if (err != 0 || num_bytes == 0) {
		//std::cout << "[클라이언트] 서버 연결 종료\n";
		isRunning = false;
		return;
	}
	//std::cout << "recv\n";
	//std::cout << "[클라이언트] 수신된 데이터 크기: " << num_bytes << " bytes\n";

	PacketType* type = reinterpret_cast<PacketType*>(recv_buffer);
	if (*type == PacketType::CHAT) {
		//ChatPacket* chatPacket = reinterpret_cast<ChatPacket*>(recv_buffer);
		ChatPacket_StoC* chatPacket = reinterpret_cast<ChatPacket_StoC*>(recv_buffer);
		std::string msg{ chatPacket->message,num_bytes - sizeof(PacketType) - sizeof(unsigned int) };
		std::cout << "[서버]채팅-" << chatPacket->id << ":" << msg << std::endl;
	}
	else if (*type == PacketType::MOVE) {
		MovePacket_StoC* movePacket = reinterpret_cast<MovePacket_StoC*>(recv_buffer);
		//std::cout << "[서버]이동: " << movePacket->id << ":" << movePacket->x << "," << movePacket->y<<"," << movePacket->z << std::endl;
		
		if (!ID_List.contains(movePacket->id)) {
			ID_List.insert(movePacket->id);
			scene.AddObject(new OtherPlayer, std::to_string(movePacket->id), LAYER1);
		}

		else
			if (auto Found = scene.Find(std::to_string(movePacket->id)); Found)
				Found->InputPosition(XMFLOAT3(movePacket->x, movePacket->y, movePacket->z));
	}
	else if (*type == PacketType::VIEW_ANGLE) {
		ViewingAnglePacket_StoC* viewAnglePacket = reinterpret_cast<ViewingAnglePacket_StoC*>(recv_buffer);
		//std::cout << "[서버]시선: " << viewAnglePacket->id << ":" << viewAnglePacket->x << "," << viewAnglePacket->y << "," << viewAnglePacket->z << std::endl;
		
		if (!ID_List.contains(viewAnglePacket->id)) {
			ID_List.insert(viewAnglePacket->id);
			scene.AddObject(new OtherPlayer, std::to_string(viewAnglePacket->id), LAYER1);
		}

		else
			if (auto Found = scene.Find(std::to_string(viewAnglePacket->id)); Found)
				Found->InputRotation(XMFLOAT3(viewAnglePacket->x, viewAnglePacket->y, viewAnglePacket->z));
	}
	else if (*type == PacketType::ENTER) {
		EnterRoomPacket* EnterPacket = reinterpret_cast<EnterRoomPacket*>(recv_buffer);
		std::cout <<"MYID-"<< EnterPacket->myID << " / roomID: " << EnterPacket->roomID << std::endl;///룸 id가 0일시 만들어 진것이 아님 대기중인 상태임
		if (0 != EnterPacket->roomID) {
			enter_room = true;
		}
		else {
			std::cout << "대기중.." << std::endl;
		}
	}
	else if (*type == PacketType::NEW_CLIENT) {
		NewClientPacket* newClientPacket = reinterpret_cast<NewClientPacket*>(recv_buffer);
		std::cout << "새로운 클라들어옴!:" << newClientPacket->id <<std::endl;

	//	std::cout << "ID: " << newClientPacket->id << std::endl;
	
		//player_enter = true;
		//enter_player_id = newClientPacket->id;
	}
	else if (*type == PacketType::EXISTING_CLIENTS) { 
		ExistingClientsDataPacket* pkt = reinterpret_cast<ExistingClientsDataPacket*>(recv_buffer);
		for (unsigned int i = 0; i < pkt->count; ++i) {
			auto& info = pkt->clients[i];
			std::cout << "[초기화] 클라이언트 " << info.id
				<< " 위치: " << info.x << "," << info.y << "," << info.z
				<< " 시선: " << info.angle_x << "," << info.angle_y << "," << info.angle_z
				<< std::endl;

			// TODO: ID에 해당하는 게임 객체 생성 또는 초기화
		}
	}

	// 다음 수신 요청
	recv_wsabuf[0].buf = recv_buffer;
	recv_wsabuf[0].len = sizeof(recv_buffer);
	DWORD recv_flag = 0;
	ZeroMemory(&recv_over, sizeof(recv_over));

	int result = WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
	if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
		std::cerr << "[클라이언트] 데이터 수신 오류\n";
		isRunning = false;
	}
}

// 데이터 전송 콜백 함수
void CALLBACK SendCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED p_over, DWORD flag) {
	if (err != 0 || num_bytes == 0) {
		std::cerr << "[클라이언트] 데이터 전송 실패\n";
		isRunning = false;
	}
//	std::cout << "send\n";
	recv_wsabuf[0].len = sizeof(recv_buffer);
	recv_wsabuf[0].buf = recv_buffer;
	DWORD recv_flag = 0;
	ZeroMemory(&recv_over, sizeof(recv_over));
	WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
}

// 이동 패킷 전송 함수
void SendMovePacket(float x, float y,float z) {
	if (enter_room) {
		MovePacket_CtoS movePacket = {};
		movePacket.type = PacketType::MOVE;
		movePacket.x = x;
		movePacket.y = y;
		movePacket.z = z;

		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(&movePacket);
		wsaBuf.len = sizeof(MovePacket_CtoS);

		// WSAOVERLAPPED 구조체를 동적 할당
		WSAOVERLAPPED* send_over = new WSAOVERLAPPED;
		ZeroMemory(send_over, sizeof(WSAOVERLAPPED));

		DWORD bytesSent = 0;

		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, send_over, SendCallback);//비동기io
		if (result == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING) {
				std::cerr << "[클라이언트] 이동 패킷 전송 오류: " << err << "\n";
				delete send_over;  // 오류 발생 시 할당 해제
			}
		}
	}
}
void SendViewingAnglePacket(float x, float y, float z) {
	if (enter_room) {
		ViewingAnglePacket_CtoS viewAnglePacket = {};
		viewAnglePacket.type = PacketType::VIEW_ANGLE;
		viewAnglePacket.x = x;
		viewAnglePacket.y = y;
		viewAnglePacket.z = z;
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(&viewAnglePacket);
		wsaBuf.len = sizeof(ViewingAnglePacket_CtoS);

		// WSAOVERLAPPED 구조체를 동적 할당
		WSAOVERLAPPED* send_over = new WSAOVERLAPPED;
		ZeroMemory(send_over, sizeof(WSAOVERLAPPED));

		DWORD bytesSent = 0;

		int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, send_over, SendCallback);//비동기io
		if (result == SOCKET_ERROR) {
			int err = WSAGetLastError();
			if (err != WSA_IO_PENDING) {
				std::cerr << "[클라이언트] 이동 패킷 전송 오류: " << err << "\n";
				delete send_over;  // 오류 발생 시 할당 해제
			}
		}
	}
}
// 채팅 패킷 전송 함수
void SendChatPacket(const char* message) {
	if (enter_room) {
		ChatPacket_CtoS chatPacket = {};
		chatPacket.type = PacketType::CHAT;
		int msg_size = strlen(message);
		memcpy(chatPacket.message, message, msg_size);

		WSABUF wsaBuf[1];
		wsaBuf[0].buf = reinterpret_cast<char*>(&chatPacket);
		wsaBuf[0].len = sizeof(PacketType) + msg_size;

		WSAOVERLAPPED send_over = { 0 };

		int result = WSASend(clientSocket, wsaBuf, 1, NULL, 0, &send_over, SendCallback);//동기 io
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 채팅 패킷 전송 오류\n";
		}
	}
}


int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);

Framework framework;

// 프레임워크, 카메라, 마우스유틸, 터레인 유틸은 전역 객체이다. 즉, 해당되는 헤더파일만 포함하면 어디서든지 사용 가능하다
Scene scene;
Camera camera;
MouseUtil mouse;

// 전역 커맨드 리스트
ID3D12GraphicsCommandList* GlobalCommandList;

// 오브젝트 쉐이더 루트 시그니처
ID3D12RootSignature* ObjectShaderRootSignature;

// 이미지 쉐이더 루트 시그니처
ID3D12RootSignature* ImageShaderRootSignature;

// 선 쉐이더 루트 시그니처
ID3D12RootSignature* LineShaderRootSignature;

// 바운드박스 루트 시그니처
ID3D12RootSignature* BoundboxShaderRootSignature;

// 전역 HWND
HWND GlobalHWND;

HINSTANCE						AppInstance;
TCHAR							Title[MAX_LOADSTRING];
TCHAR							WindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG Messege;
	HACCEL AccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, Title, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT045, WindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
		return(FALSE);

	AccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT045));

	if (useServer) {

		//if (lpCmdLine == NULL || _tcslen(lpCmdLine) == 0) {
		//	MessageBox(NULL, _T("서버 IP 주소를 인자로 입력하세요.\n예: program.exe 127.0.0.1"), _T("오류"), MB_OK);
		//	return -1;
		//}

		char ipStr[64] = { 0 };

#ifdef UNICODE
		// 유니코드 → 멀티바이트 변환
		WideCharToMultiByte(CP_ACP, 0, lpCmdLine, -1, ipStr, sizeof(ipStr), NULL, NULL);
#else
		// 멀티바이트는 바로 복사
		strncpy(ipStr, lpCmdLine, sizeof(ipStr) - 1);
#endif
		// 윈속 초기화
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		// 소켓 생성 및 서버 연결
		clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		SOCKADDR_IN serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		//inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);//한 컴퓨터에서 실행할때
		inet_pton(AF_INET, ipStr, &serverAddr.sin_addr);//cmd에서 ip입력할때 
		//std::cout << ipStr << std::endl;
		if (WSAConnect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
			std::cerr << "[클라이언트] 서버 연결 실패\n";
			return -1;
		}
		std::cout << "[클라이언트] 서버에 연결됨\n";

		// 첫 번째 데이터 수신 요청
		recv_wsabuf[0].buf = recv_buffer;
		recv_wsabuf[0].len = sizeof(recv_buffer);
		DWORD recv_flag = 0;
		ZeroMemory(&recv_over, sizeof(recv_over));

		int result = WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
		if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			std::cerr << "[클라이언트] 첫 번째 데이터 수신 오류\n";
			return -1;
		}
	}

	while (true) {
		if (::PeekMessage(&Messege, NULL, 0, 0, PM_REMOVE)) {
			if (Messege.message == WM_QUIT)
				break;

			if (!::TranslateAccelerator(Messege.hwnd, AccelTable, &Messege)) {
				::TranslateMessage(&Messege);
				::DispatchMessage(&Messege);
			}
		}

		else {
			framework.Update();
		}
		// 비동기 I/O 콜백 실행
		SleepEx(0, TRUE);
	}

	framework.Destroy();

	return((int)Messege.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEX Wcex;

	Wcex.cbSize = sizeof(WNDCLASSEX);

	Wcex.style = CS_HREDRAW | CS_VREDRAW;
	Wcex.lpfnWndProc = WndProc;
	Wcex.cbClsExtra = 0;
	Wcex.cbWndExtra = 0;
	Wcex.hInstance = hInstance;
	Wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT045));
	Wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	Wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	Wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_LABPROJECT045);
	Wcex.lpszClassName = WindowClass;
	Wcex.hIconSm = ::LoadIcon(Wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassEx(&Wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	AppInstance = hInstance;
	DWORD WindowStyle{};
	RECT Rect{};

	Rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	WindowStyle = WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	AdjustWindowRect(&Rect, WindowStyle, FALSE);

	HWND MainWnd = CreateWindow(
		WindowClass,
		Title,
		WindowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		Rect.right - Rect.left,
		Rect.bottom - Rect.top,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!MainWnd)
		return(FALSE);

	framework.Create(hInstance, MainWnd);

	::ShowWindow(MainWnd, nCmdShow);
	::UpdateWindow(MainWnd);

	// Config.h에서 전체화면 모드를 활성화 했을 경우 바로 전체화면으로 전환된다
	if (!START_WITH_FULL_SCREEN)
		framework.SwitchToWindowMode(MainWnd);

	return(TRUE);
}

// F11키를 누를 시 창모드<->전체화면 모드 전환이 가능하다. 조작키는 비활성화 하거나 바꿀 수 있다.
// ChangeSwapChainState()를 사용하지 않는 이유는 해당 함수 사용 시 뷰포트의 선명도가 떨어지는 문제가 있기 때문이다.
// 창 크기를 늘려 제목표시줄을 없애는 방식으로 전체화면을 구현한다.
void DisplayStateChanger(HWND hWnd, UINT nMessageID, WPARAM wParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_F11:
			if (framework.FullScreenState)
				framework.SwitchToWindowMode(hWnd);
			else
				framework.SwitchToFullscreenMode(hWnd);
			break;
		}
		break;
	}
}

// 이 함수의 내용들은 수정할 필요 없다.
LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	int wmId, wmEvent;
	PAINTSTRUCT PaintStruct;
	HDC Hdc;
	GlobalHWND = hWnd;

	DisplayStateChanger(hWnd, nMessageID, wParam);

	switch (nMessageID) {
		// 윈도우 사이즈 변경이 감지되면 카메라 행렬을 새로 업데이트한다.
	case WM_SIZE:
	{
		SCREEN_WIDTH = LOWORD(lParam);
		SCREEN_HEIGHT = HIWORD(lParam);
		camera.GenerateStaticMatrix();
	}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
		scene.InputMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN: case WM_KEYUP:
		scene.InputKeyMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
		scene.InputMouseMotionMessage(hWnd);
		break;

	case WM_ACTIVATE:
		framework.WindowsMessageFunc(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId) {
		case IDM_ABOUT:
			::DialogBox(AppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			::DestroyWindow(hWnd);
			break;

		default:
			return(::DefWindowProc(hWnd, nMessageID, wParam, lParam));
		}

		break;

	case WM_PAINT:
		Hdc = ::BeginPaint(hWnd, &PaintStruct);
		EndPaint(hWnd, &PaintStruct);
		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	default:
		return(::DefWindowProc(hWnd, nMessageID, wParam, lParam));
	}

	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (nMessageID) {
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}

	return((INT_PTR)FALSE);
}

// 뷰포트 배경색을 변경한다. flaot 값 또는 rgb값을 사용할 수 있다.
void SetBackgroundColorRGB(int R, int G, int B) {
	framework.BackgroundColor.x = 1.0 / 255.0 * float(R);
	framework.BackgroundColor.y = 1.0 / 255.0 * float(G);
	framework.BackgroundColor.z = 1.0 / 255.0 * float(B);
}

void SetBackgroundColor(float R, float G, float B) {
	framework.BackgroundColor.x = float(R);
	framework.BackgroundColor.y = float(G);
	framework.BackgroundColor.z = float(B);
}