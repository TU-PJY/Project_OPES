#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>  // inet_pton을 위해 필요
#include <iostream>
#include <thread>
#include "Packet.h"
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib, "msimg32.lib") 
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3000

SOCKET clientSocket;
bool isRunning = true;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MovePacket movePacket={};
    movePacket.type = PacketType::MOVE;

    switch (uMsg) {
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP: movePacket.direction = 0; break;
        case VK_DOWN: movePacket.direction = 1; break;
        case VK_LEFT: movePacket.direction = 2; break;
        case VK_RIGHT: movePacket.direction = 3; break;
        default: return 0;
        }

        std::cout << "[클라이언트] 이동 패킷 전송: 방향 " << movePacket.direction << std::endl;
        send(clientSocket, (char*)&movePacket, sizeof(MovePacket), 0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 콘솔에서 채팅을 입력하여 전송하는 스레드
void ChatThread() {
    while (isRunning) {
        char message[MAX_SOCKBUF] = { 0 };
        std::cin.getline(message, MAX_SOCKBUF);

        ChatPacket chatPacket;
        chatPacket.type = PacketType::CHAT;
        strncpy_s(chatPacket.message, MAX_SOCKBUF, message, _TRUNCATE);

        std::cout << "[클라이언트] 채팅 전송: " << chatPacket.message << std::endl;
        int packetSize = sizeof(PacketType) + strlen(chatPacket.message) + 1;
        send(clientSocket, (char*)&chatPacket, packetSize, 0);
    }
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 윈속 초기화
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 소켓 생성 및 연결
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "서버 연결 실패\n";
        return -1;
    }
    std::cout << "서버에 연결되었습니다.\n";

    // 채팅 입력을 위한 콘솔 스레드 실행
    std::thread chatThread(ChatThread);

    // 윈도우 클래스 설정
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"GameClient"; // 수정됨
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(L"GameClient", L"테스트 클라이언트", WS_OVERLAPPEDWINDOW,  // 수정됨
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);

    MSG msg;
    while (isRunning) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        Sleep(10);
    }

    chatThread.join();
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}


