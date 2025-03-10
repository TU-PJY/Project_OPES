#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 11021
#define MAX_SOCKBUF 1024

HANDLE hIOCP;
SOCKET clientSocket;
enum class IOOperation {
    RECV,
    SEND
};
struct stOverlappedEx {
    WSAOVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[MAX_SOCKBUF];
    IOOperation operation;
};

void CALLBACK CompletionRoutine(DWORD dwError, DWORD bytesTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD flags) {
    stOverlappedEx* overlappedEx = (stOverlappedEx*)lpOverlapped;

    if (dwError != 0 || bytesTransferred == 0) {
        std::cerr << "���� ���� ����.\n";
        closesocket(clientSocket);
        WSACleanup();
        exit(1);
    }

    if (overlappedEx->operation == IOOperation::RECV) {
        overlappedEx->buffer[bytesTransferred] = '\0';
        std::cout << "���� �޽���: " << overlappedEx->buffer << std::endl;

        // �ٽ� ���� ���
        DWORD flags = 0;
        WSARecv(clientSocket, &overlappedEx->wsaBuf, 1, NULL, &flags, lpOverlapped, CompletionRoutine);
    }
}

void RegisterRecv() {
    static stOverlappedEx recvOverlapped = {};
    recvOverlapped.operation = IOOperation::RECV;
    recvOverlapped.wsaBuf.buf = recvOverlapped.buffer;
    recvOverlapped.wsaBuf.len = MAX_SOCKBUF;

    DWORD flags = 0;
    WSARecv(clientSocket, &recvOverlapped.wsaBuf, 1, NULL, &flags, &recvOverlapped.overlapped, CompletionRoutine);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "���� ���� ����\n";
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "������ ����Ǿ����ϴ�!\n";

    // IOCP �ڵ� ���� �� ���
    hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, NULL, (ULONG_PTR)clientSocket, 0);

    // ù ��° ���� ��û
    RegisterRecv();

    // �޽��� �Է� �� ����
    while (true) {
        std::string message;
        std::cout << "������ ���� �޽��� �Է� (exit �Է� �� ����): ";
        std::getline(std::cin, message);

        if (message == "exit") break;

        stOverlappedEx sendOverlapped = {};
        sendOverlapped.operation = IOOperation::SEND;
        sendOverlapped.wsaBuf.buf = sendOverlapped.buffer;
        sendOverlapped.wsaBuf.len = message.size();
        memcpy(sendOverlapped.buffer, message.c_str(), message.size());

        WSASend(clientSocket, &sendOverlapped.wsaBuf, 1, NULL, 0, &sendOverlapped.overlapped, NULL);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
