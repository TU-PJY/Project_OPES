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

    std::cout << "���� �ʱ�ȭ ����\n";
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

    std::cout << "���� ��� ����..\n";
    return true;
}

bool IOCompletionPort::StartServer() {
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 4);
    if (iocpHandle == NULL) {
        std::cerr << "[����] CreateIoCompletionPort() ����: " << GetLastError() << "\n";
        return false;
    }

    //for (int i = 0; i < MAX_WORKERTHREAD; i++) {
    //    workerThreads.emplace_back([this]() { WorkerThread(); });
    //}
    accepterThread = std::thread([this]() { AcceptThread(); });
    workerThread=std::thread([this]() { WorkThread(); });

    std::cout << "������ ���۵Ǿ����ϴ�.\n";
    return true;
}

void IOCompletionPort::AcceptThread() {
    while (isRunning) {
        SOCKADDR_IN clientAddr;
        int addrLen = sizeof(SOCKADDR_IN);

        // Ŭ���̾�Ʈ ����ü�� ���� �Ҵ��Ͽ� ����
        stClientInfo* newClient = new stClientInfo();
        newClient->socketClient = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
        if (newClient->socketClient == INVALID_SOCKET) {
            std::cerr << "[����] accept() ����\n";
            delete newClient;  // �޸� ����
            continue;
        }

        CreateIoCompletionPort((HANDLE)newClient->socketClient, iocpHandle, (ULONG_PTR)newClient, 0);
        clients.push_back(newClient);

        std::cout << "���ο� Ŭ���̾�Ʈ ����! ���� Ŭ���̾�Ʈ ��: " << clients.size() << "\n";

        RegisterRecv(newClient);
    }
}


void IOCompletionPort::RegisterRecv(stClientInfo* client) {
    if (!client || client->socketClient == INVALID_SOCKET) {
        std::cerr << "[����] ��ȿ���� ���� Ŭ���̾�Ʈ ����\n";
        return;
    }

    DWORD flags = 0;
    DWORD bytesReceived = 0;
    client->recvOverlapped.operation = IOOperation::RECV;
    client->recvOverlapped.wsaBuf.len = MAX_SOCKBUF;
    client->recvOverlapped.wsaBuf.buf = client->recvOverlapped.buffer;

    int result = WSARecv(client->socketClient, &client->recvOverlapped.wsaBuf, 1, &bytesReceived, &flags, &client->recvOverlapped.overlapped, NULL);
    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[����] WSARecv ����: " << WSAGetLastError() << std::endl;
        closesocket(client->socketClient);
        RemoveClient(client);
    }
}
void IOCompletionPort::RemoveClient(stClientInfo* client) {
    std::lock_guard<std::mutex> lock(clientMutex);

    auto it = std::find(clients.begin(), clients.end(), client);
    if (it != clients.end()) {
        clients.erase(it);
        delete client;  // ���� �Ҵ�� Ŭ���̾�Ʈ ����
    }
}



void IOCompletionPort::SendData(stClientInfo* client, const char* message, int length) {
    client->sendOverlapped.operation = IOOperation::SEND;
    client->sendOverlapped.wsaBuf.len = length;
    client->sendOverlapped.wsaBuf.buf = client->sendOverlapped.buffer;
    memcpy(client->sendOverlapped.buffer, message, length);

    WSASend(client->socketClient, &client->sendOverlapped.wsaBuf, 1, NULL, 0, &client->sendOverlapped.overlapped, NULL);
}

void IOCompletionPort::WorkThread() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;

    while (isRunning) {
        BOOL result = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!result || bytesTransferred == 0) continue;

        stOverlappedEx* pOverlappedEx = reinterpret_cast<stOverlappedEx*>(overlapped);
        stClientInfo* client = reinterpret_cast<stClientInfo*>(completionKey);

        PacketType* packetType = reinterpret_cast<PacketType*>(pOverlappedEx->buffer);

        std::cout << "[�����] ���ŵ� ��Ŷ Ÿ��: " << static_cast<int>(*packetType)
            << ", ���� ����Ʈ: " << bytesTransferred << std::endl;

        if (*packetType == PacketType::MOVE) {
           // if (bytesTransferred < sizeof(MovePacket)) {
           //     std::cerr << "[����] MOVE ��Ŷ ũ�� ����: " << bytesTransferred << " bytes" << std::endl;
           //     continue;
           // }

            MovePacket* movePacket = reinterpret_cast<MovePacket*>(pOverlappedEx->buffer);

            // �̵� ó��
            switch (movePacket->direction) {
            case 0: client->y -= 1; break; // UP
            case 1: client->y += 1; break; // DOWN
            case 2: client->x -= 1; break; // LEFT
            case 3: client->x += 1; break; // RIGHT
            default: continue;
            }

            std::cout << "[�̵�] Ŭ���̾�Ʈ " << client->id
                << " ��ġ: (" << client->x << ", " << client->y << ")\n";

            // �̵� ��Ŷ�� ��� Ŭ���̾�Ʈ���� ����
            for (stClientInfo* otherClient : clients) {
                if (otherClient != client) { // ��Ŷ�� ���� Ŭ���̾�Ʈ���Դ� �ٽ� �������� ����
                    SendData(otherClient, reinterpret_cast<char*>(movePacket), sizeof(MovePacket));
                }
            }
        }
        else if (*packetType == PacketType::CHAT) {
            //if (bytesTransferred < sizeof(ChatPacket)) {
            //    std::cerr << "[����] CHAT ��Ŷ ũ�� ����: " << bytesTransferred << " bytes" << std::endl;
            //    continue;
            //}

            ChatPacket* chatPacket = reinterpret_cast<ChatPacket*>(pOverlappedEx->buffer);
            std::cout << "[ä��] Ŭ���̾�Ʈ " << client->id << ": " << chatPacket->message << std::endl;

            // ä�� ��Ŷ�� ��� Ŭ���̾�Ʈ���� ����
            for (stClientInfo* otherClient : clients) {
                if (otherClient != client) { // ��Ŷ�� ���� Ŭ���̾�Ʈ���Դ� �ٽ� �������� ����
                    SendData(otherClient, reinterpret_cast<char*>(chatPacket), sizeof(MovePacket));
                }
            }
        }

        RegisterRecv(client);  // �ٽ� ���� ���
    }
}


void IOCompletionPort::DestroyThread() {
    isRunning = false;
    CloseHandle(iocpHandle);
    closesocket(listenSocket);

    //for (auto& th : workerThreads) {
    //    if (th.joinable()) th.join();
    //} 
    if (workerThread.joinable()) workerThread.join();

    if (accepterThread.joinable()) accepterThread.join();

    std::cout << "���� ���� �Ϸ�.\n";
}
