#include <winsock2.h>
#include <windows.h>
#include <Ws2tcpip.h>  
#include <iostream>
#include <conio.h>
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include "Packet.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "msimg32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3000

SOCKET clientSocket;
bool isRunning = true;
bool enter_room = false;
WSABUF recv_wsabuf[1];
char recv_buffer[MAX_SOCKBUF];
WSAOVERLAPPED recv_over;

// ������ ���� �ݹ� �Լ�
void CALLBACK RecvCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED p_over, DWORD flag) {
    if (err != 0 || num_bytes == 0) {
        std::cout << "[Ŭ���̾�Ʈ] ���� ���� ����\n";
        isRunning = false;
        return;
    }
    std::cout << "recv\n";
    std::cout << "[Ŭ���̾�Ʈ] ���ŵ� ������ ũ��: " << num_bytes << " bytes\n";

    PacketType* type = reinterpret_cast<PacketType*>(recv_buffer);
    if (*type == PacketType::CHAT) {
        //ChatPacket* chatPacket = reinterpret_cast<ChatPacket*>(recv_buffer);
        ChatPacket_R* chatPacket = reinterpret_cast<ChatPacket_R*>(recv_buffer);
        std::string msg{ chatPacket->message,num_bytes - sizeof(PacketType) - sizeof(unsigned int) };
        std::cout << "[����]  " << chatPacket->id << ":" << msg << std::endl;
    }
    else if (*type == PacketType::MOVE) {
        MovePacket_R* movePacket = reinterpret_cast<MovePacket_R*>(recv_buffer);
        std::cout << "[����]  " << movePacket->id << ":" << movePacket->x << "," << movePacket->y << std::endl;
    }
    else if (*type == PacketType::ENTER) {
        EnterRoomPacket* EnterPacket = reinterpret_cast<EnterRoomPacket*>(recv_buffer);
        std::cout << "[����]  " << EnterPacket->roomID << std::endl;
        if (0 != EnterPacket->roomID) {
            enter_room = true;
        }
        else {
            std::cout << "�����.." << std::endl;
        }
    }

    // ���� ���� ��û
    recv_wsabuf[0].buf = recv_buffer;
    recv_wsabuf[0].len = sizeof(recv_buffer);
    DWORD recv_flag = 0;
    ZeroMemory(&recv_over, sizeof(recv_over));

    int result = WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[Ŭ���̾�Ʈ] ������ ���� ����\n";
        isRunning = false;
    }
}

// ������ ���� �ݹ� �Լ�
void CALLBACK SendCallback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED p_over, DWORD flag) {
    if (err != 0 || num_bytes == 0) {
        std::cerr << "[Ŭ���̾�Ʈ] ������ ���� ����\n";
        isRunning = false;
    }
    std::cout << "send\n";
    recv_wsabuf[0].len = sizeof(recv_buffer);
    recv_wsabuf[0].buf = recv_buffer;
    DWORD recv_flag = 0;
    ZeroMemory(&recv_over, sizeof(recv_over));
    WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
}

// �̵� ��Ŷ ���� �Լ�
void SendMovePacket(char direction) {
    if (enter_room) {
        MovePacket_S movePacket = {};
        movePacket.type = PacketType::MOVE;
        movePacket.direction = direction;

        WSABUF wsaBuf;
        wsaBuf.buf = reinterpret_cast<char*>(&movePacket);
        wsaBuf.len = sizeof(MovePacket_S);

        // WSAOVERLAPPED ����ü�� ���� �Ҵ�
        WSAOVERLAPPED* send_over = new WSAOVERLAPPED;
        ZeroMemory(send_over, sizeof(WSAOVERLAPPED));

        DWORD bytesSent = 0;

        int result = WSASend(clientSocket, &wsaBuf, 1, &bytesSent, 0, send_over, SendCallback);//�񵿱�io
        if (result == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                std::cerr << "[Ŭ���̾�Ʈ] �̵� ��Ŷ ���� ����: " << err << "\n";
                delete send_over;  // ���� �߻� �� �Ҵ� ����
            }
        }
    }
}

// ä�� ��Ŷ ���� �Լ�
void SendChatPacket(const char* message) {
    if (enter_room) {
        ChatPacket_S chatPacket = {};
        chatPacket.type = PacketType::CHAT;
        int msg_size = strlen(message);
        memcpy(chatPacket.message, message, msg_size);

        WSABUF wsaBuf[1];
        wsaBuf[0].buf = reinterpret_cast<char*>(&chatPacket);
        wsaBuf[0].len = sizeof(PacketType) + msg_size;

        WSAOVERLAPPED send_over = { 0 };

        int result = WSASend(clientSocket, wsaBuf, 1, NULL, 0, &send_over, SendCallback);//���� io
        if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            std::cerr << "[Ŭ���̾�Ʈ] ä�� ��Ŷ ���� ����\n";
        }
    }
}

// ������ ���ν���
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        //case WM_DESTROY:
        //    isRunning = false;
        //    PostQuitMessage(0);
        //    return 0;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP:    SendMovePacket(0); break;
        case VK_DOWN:  SendMovePacket(1); break;
        case VK_LEFT:  SendMovePacket(2); break;
        case VK_RIGHT: SendMovePacket(3); break;
        default: return 0;
        }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// ������ ���� �� ��Ʈ��ũ �ʱ�ȭ
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ���� �ʱ�ȭ
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // ���� ���� �� ���� ����
    clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (WSAConnect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        std::cerr << "[Ŭ���̾�Ʈ] ���� ���� ����\n";
        return -1;
    }
    std::cout << "[Ŭ���̾�Ʈ] ������ �����\n";

    // ù ��° ������ ���� ��û
    recv_wsabuf[0].buf = recv_buffer;
    recv_wsabuf[0].len = sizeof(recv_buffer);
    DWORD recv_flag = 0;
    ZeroMemory(&recv_over, sizeof(recv_over));

    int result = WSARecv(clientSocket, recv_wsabuf, 1, NULL, &recv_flag, &recv_over, RecvCallback);
    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[Ŭ���̾�Ʈ] ù ��° ������ ���� ����\n";
        return -1;
    }

    // ������ Ŭ���� ����
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"GameClient";
    RegisterClass(&wc);

    // ������ ����
    HWND hwnd = CreateWindow(L"GameClient", L"�׽�Ʈ Ŭ���̾�Ʈ", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    //while (1) {
    //    if (enter_room) break;
    //    SleepEx(0, TRUE);
    //}
     // �޽��� ����
    MSG msg;
    while (isRunning) {

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // �ܼ� ä�� �Է� ó��
        if (_kbhit()) {
            char message[MAX_SOCKBUF];
            std::cin.getline(message, MAX_SOCKBUF);
            SendChatPacket(message);
            std::cout << "loop\n";
        }

        // �񵿱� I/O �ݹ� ����
        SleepEx(0, TRUE);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
