#include "IOCompletionPort.h"
#include <iostream>

IOCompletionPort::IOCompletionPort() {}

IOCompletionPort::~IOCompletionPort() {
    WSACleanup();
}

bool IOCompletionPort::InitSocket() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }

    listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return false;
    }

    std::cout << "소켓 초기화 성공\n";
    return true;
}

bool IOCompletionPort::BindandListen(int nBindPort) {
    SOCKADDR_IN serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(nBindPort);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        return false;
    }

    if (listen(listenSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        return false;
    }

    std::cout << "서버 등록 성공..\n";
    return true;
}

bool IOCompletionPort::StartServer(const UINT32 maxClientCount) {
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, MAX_WORKERTHREAD);
    if (iocpHandle == NULL) {
        std::cerr << "[에러] CreateIoCompletionPort() 실패: " << GetLastError() << "\n";
        return false;
    }

    for (int i = 0; i < MAX_WORKERTHREAD; i++) {
        workerThreads.emplace_back([this]() { WorkerThread(); });
    }

    accepterThread = std::thread([this]() { AcceptThread(); });

    std::cout << "서버가 시작되었습니다.\n";
    return true;
}

void IOCompletionPort::AcceptThread() {
    while (isRunning) {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(SOCKADDR_IN);

        stClientInfo newClient;
        newClient.socketClient = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (newClient.socketClient == INVALID_SOCKET) {
            std::cerr << "[에러] accept() 실패\n";
            continue;
        }

        CreateIoCompletionPort((HANDLE)newClient.socketClient, iocpHandle, (ULONG_PTR)&newClient, 0);
        clients.push_back(newClient);

        std::cout << "새로운 클라이언트 접속! 현재 클라이언트 수: " << clients.size() << "\n";

        RegisterRecv(&clients.back());
    }
}

void IOCompletionPort::RegisterRecv(stClientInfo* client) {
    DWORD flags = 0;
    client->recvOverlapped.operation = IOOperation::RECV;
    client->recvOverlapped.wsaBuf.len = MAX_SOCKBUF;
    client->recvOverlapped.wsaBuf.buf = client->recvOverlapped.buffer;

    WSARecv(client->socketClient, &client->recvOverlapped.wsaBuf, 1, NULL, &flags, &client->recvOverlapped.overlapped, NULL);
}

void IOCompletionPort::SendData(stClientInfo* client, const char* message, int length) {
    client->sendOverlapped.operation = IOOperation::SEND;
    client->sendOverlapped.wsaBuf.len = length;
    client->sendOverlapped.wsaBuf.buf = client->sendOverlapped.buffer;
    memcpy(client->sendOverlapped.buffer, message, length);

    WSASend(client->socketClient, &client->sendOverlapped.wsaBuf, 1, NULL, 0, &client->sendOverlapped.overlapped, NULL);
}

void IOCompletionPort::WorkerThread() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;

    while (isRunning) {
        BOOL result = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!result || bytesTransferred == 0) continue;

        stOverlappedEx* pOverlappedEx = reinterpret_cast<stOverlappedEx*>(overlapped);
        stClientInfo* client = reinterpret_cast<stClientInfo*>(completionKey);

        if (pOverlappedEx->operation == IOOperation::RECV) {
            std::cout << "받은 메시지: " << pOverlappedEx->buffer << std::endl;
            RegisterRecv(client);
        }
    }
}

void IOCompletionPort::DestroyThread() {
    isRunning = false;
    CloseHandle(iocpHandle);
    closesocket(listenSocket);

    for (auto& th : workerThreads) {
        if (th.joinable()) th.join();
    }

    if (accepterThread.joinable()) accepterThread.join();

    std::cout << "서버 종료 완료.\n";
}
