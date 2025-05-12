#include "IOCompletionPort.h"
#include"Packet.h"
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

bool IOCompletionPort::BindandListen() {
    SOCKADDR_IN serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
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
    workerThread = std::thread([this]() { WorkThread(); });

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

        idCount++;
        newClient->id = idCount;
        clients.push_back(newClient);
        NotifyOthersAboutNewClient(newClient);
        SendExistingClientsToNewClient(newClient);
        std::cout << "���ο� Ŭ���̾�Ʈ ����! ���� Ŭ���̾�Ʈ ��: " << clients.size() << "\n";
        //
        std::lock_guard<std::mutex> lock(waitMutex);
        waitingClients.push_back(newClient);
        if (waitingClients.size() >= 3) {
            std::vector<stClientInfo*> roomMembers(waitingClients.begin(), waitingClients.begin() + 3);
            waitingClients.erase(waitingClients.begin(), waitingClients.begin() + 3);

            CreateRoom(roomMembers);
            for (auto& client : roomMembers) {
                SendData_EnterRoom(client);
                RegisterRecv(client);
            }
        }
        else {
            SendData_EnterRoom(newClient);
            RegisterRecv(newClient);
        }


    }
}
void IOCompletionPort::SendExistingClientsToNewClient(stClientInfo* newClient) {
    ExistingClientsDataPacket packet;
    packet.type = PacketType::EXISTING_CLIENTS;
    packet.count = 0;

    for (auto& client : clients) {
        if (client != newClient && client->roomID == newClient->roomID) {
            auto& dst = packet.clients[packet.count++];
            dst.id = client->id;
            dst.x = client->x;
            dst.y = client->y;
            dst.z = client->z;
            dst.angle_x = client->angle_x;
            dst.angle_y = client->angle_y;
            dst.angle_z = client->angle_z;
        }
    }

    if (packet.count == 0)
        return;

    newClient->sendOverlapped.operation = IOOperation::SEND;
    ZeroMemory(&newClient->sendOverlapped.overlapped, sizeof(newClient->sendOverlapped.overlapped));
    newClient->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&packet);
    newClient->sendOverlapped.wsaBuf.len = sizeof(PacketType) + sizeof(unsigned int) + packet.count * sizeof(packet.clients[0]);

    DWORD size_sent = 0;
    int ret = WSASend(newClient->socketClient, &newClient->sendOverlapped.wsaBuf, 1, &size_sent, 0, &newClient->sendOverlapped.overlapped, NULL);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[����] WSASend(EXISTING_CLIENTS) ����: " << WSAGetLastError() << std::endl;
        closesocket(newClient->socketClient);
        RemoveClient(newClient);
    }
}
void IOCompletionPort::NotifyOthersAboutNewClient(stClientInfo* newClient) {
    NewClientPacket pkt;
    pkt.type = PacketType::NEW_CLIENT;
    pkt.id = newClient->id;

    for (auto& other : clients) {
        if (other != newClient && other->roomID == newClient->roomID) {
            other->sendOverlapped.operation = IOOperation::SEND;
            ZeroMemory(&other->sendOverlapped.overlapped, sizeof(other->sendOverlapped.overlapped));
            other->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&pkt);
            other->sendOverlapped.wsaBuf.len = sizeof(pkt);

            DWORD sent = 0;
            int ret = WSASend(other->socketClient, &other->sendOverlapped.wsaBuf, 1, &sent, 0, &other->sendOverlapped.overlapped, NULL);
            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                std::cerr << "[����] WSASend ���� (NEW_CLIENT): " << WSAGetLastError() << "\n";
                closesocket(other->socketClient);
                RemoveClient(other);
            }
        }
    }
}
void IOCompletionPort::CreateRoom(const std::vector<stClientInfo*>& members) {
    std::lock_guard<std::mutex> lock(roomMutex);

    Room newRoom;
    nextRoomID++;
    newRoom.roomID = nextRoomID;
    newRoom.clients = members;

    for (auto* client : members) {
        client->roomID = newRoom.roomID;
        // �� ���� �޽��� ����
        //SendRoomEnterMessage(client, newRoom.roomID);
    }

    rooms[newRoom.roomID] = newRoom;
    std::cout << "create room!: " << newRoom.roomID << std::endl;
}
void IOCompletionPort::SendData_EnterRoom(stClientInfo* recvingClient) {
    recvingClient->sendOverlapped.operation = IOOperation::SEND;
    ZeroMemory(&recvingClient->sendOverlapped.overlapped, sizeof(recvingClient->sendOverlapped.overlapped));
    EnterRoomPacket p;
    p.type = PacketType::ENTER;
    p.roomID = recvingClient->roomID;
    p.myID = recvingClient->id;
    recvingClient->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&p);
    recvingClient->sendOverlapped.wsaBuf.len = sizeof(EnterRoomPacket);

    DWORD size_sent = 0;
    int ret = WSASend(recvingClient->socketClient, &recvingClient->sendOverlapped.wsaBuf, 1, &size_sent, 0, &recvingClient->sendOverlapped.overlapped, NULL);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[����] WSASend ����: " << WSAGetLastError() << std::endl;
        closesocket(recvingClient->socketClient);
        RemoveClient(recvingClient);
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

    clients.erase(std::remove_if(clients.begin(), clients.end(),
        [&](stClientInfo* c) {
            if (c == client) {
                //delete c;
                return true;
            }
            return false;
        }), clients.end());
    waitingClients.erase(std::remove_if(waitingClients.begin(), waitingClients.end(),
        [&](stClientInfo* c) {
            if (c == client) {
                delete c;
                return true;
            }
            return false;
        }), waitingClients.end());
}

void IOCompletionPort::SendData(stClientInfo* sendingClient, stClientInfo* recvingClient, const char* message, int length) {
    recvingClient->sendOverlapped.operation = IOOperation::SEND;
    ZeroMemory(&recvingClient->sendOverlapped.overlapped, sizeof(recvingClient->sendOverlapped.overlapped));
    ChatPacket_StoC chatPacket = {};
    chatPacket.type = PacketType::CHAT;
    int msg_size = length - sizeof(PacketType);
    memcpy(chatPacket.message, message, msg_size);
    chatPacket.id = sendingClient->id;

    recvingClient->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&chatPacket);
    recvingClient->sendOverlapped.wsaBuf.len = length + sizeof(unsigned int);

    DWORD size_sent = 0;
    int ret = WSASend(recvingClient->socketClient, &recvingClient->sendOverlapped.wsaBuf, 1, &size_sent, 0, &recvingClient->sendOverlapped.overlapped, NULL);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[����] WSASend ����: " << WSAGetLastError() << std::endl;
        closesocket(recvingClient->socketClient);
        RemoveClient(recvingClient);
    }

}
void IOCompletionPort::SendData_Move(stClientInfo* sendingClient, stClientInfo* recvingClient) {
    recvingClient->sendOverlapped.operation = IOOperation::SEND;
    ZeroMemory(&recvingClient->sendOverlapped.overlapped, sizeof(recvingClient->sendOverlapped.overlapped));
    MovePacket_StoC movePacket = {};
    movePacket.type = PacketType::MOVE;
    movePacket.x = sendingClient->x;
    movePacket.y = sendingClient->y;
    movePacket.z = sendingClient->z;
    movePacket.id = sendingClient->id;

    recvingClient->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&movePacket);
    recvingClient->sendOverlapped.wsaBuf.len = sizeof(movePacket);

    DWORD size_sent = 0;
    int ret = WSASend(recvingClient->socketClient, &recvingClient->sendOverlapped.wsaBuf, 1, &size_sent, 0, &recvingClient->sendOverlapped.overlapped, NULL);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[����] WSASend ����: " << WSAGetLastError() << std::endl;
        closesocket(recvingClient->socketClient);
        RemoveClient(recvingClient);
    }

}
void IOCompletionPort::SendData_ViewAngle(stClientInfo* sendingClient, stClientInfo* recvingClient) {
    recvingClient->sendOverlapped.operation = IOOperation::SEND;
    ZeroMemory(&recvingClient->sendOverlapped.overlapped, sizeof(recvingClient->sendOverlapped.overlapped));
    ViewingAnglePacket_StoC viewAnglePacket = {};
    viewAnglePacket.type = PacketType::VIEW_ANGLE;
    viewAnglePacket.x = sendingClient->angle_x;
    viewAnglePacket.y = sendingClient->angle_y;
    viewAnglePacket.z = sendingClient->angle_z;
    viewAnglePacket.id = sendingClient->id;

    recvingClient->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&viewAnglePacket);
    recvingClient->sendOverlapped.wsaBuf.len = sizeof(viewAnglePacket);

    DWORD size_sent = 0;
    int ret = WSASend(recvingClient->socketClient, &recvingClient->sendOverlapped.wsaBuf, 1, &size_sent, 0, &recvingClient->sendOverlapped.overlapped, NULL);
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[����] WSASend ����: " << WSAGetLastError() << std::endl;
        closesocket(recvingClient->socketClient);
        RemoveClient(recvingClient);
    }

}
void IOCompletionPort::WorkThread() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;

    while (isRunning) {
        BOOL result = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        if (!result || bytesTransferred == 0) {
            // Ŭ���̾�Ʈ ���� ����
            stClientInfo* client = reinterpret_cast<stClientInfo*>(completionKey);
            std::cerr << "[���� ����] Ŭ���̾�Ʈ ID " << client->id << " ���� ����\n";
            closesocket(client->socketClient);
            RemoveClient(client);
            continue;
        }

        stOverlappedEx* pOverlappedEx = reinterpret_cast<stOverlappedEx*>(overlapped);
        if (pOverlappedEx->operation == IOOperation::RECV) {
            stClientInfo* client = reinterpret_cast<stClientInfo*>(completionKey);

            PacketType* packetType = reinterpret_cast<PacketType*>(pOverlappedEx->buffer);

            std::cout << "[�����] ���ŵ� ��Ŷ Ÿ��: " << static_cast<int>(*packetType)
                << ", ���� ����Ʈ: " << bytesTransferred << std::endl;

            if (*packetType == PacketType::MOVE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[����] MOVE ��Ŷ ũ�� ����: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                MovePacket_CtoS* movePacket = reinterpret_cast<MovePacket_CtoS*>(pOverlappedEx->buffer);

                // �̵� ó��
               //switch (movePacket->direction) {
               //case 0: client->y -= 1; break; // UP
               //case 1: client->y += 1; break; // DOWN
               //case 2: client->x -= 1; break; // LEFT
               //case 3: client->x += 1; break; // RIGHT
               //default: continue;
               //}
                //movePacket->id = client->id;
                client->x = movePacket->x;
                client->y = movePacket->y;
                client->z = movePacket->z;
                std::cout << "[�̵�] Ŭ���̾�Ʈ " << client->id
                    << " ��ġ: (" << client->x << ", " << client->y << ", " << client->z<<")\n";

                // �̵� ��Ŷ�� ��� Ŭ���̾�Ʈ���� ����
                for (stClientInfo* otherClient : clients) {
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // ��Ŷ�� ���� Ŭ���̾�Ʈ���Դ� �ٽ� �������� ����
                        SendData_Move(client, otherClient);
                    }
                }
            }
            else if (*packetType == PacketType::VIEW_ANGLE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[����] MOVE ��Ŷ ũ�� ����: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                ViewingAnglePacket_CtoS* viewAnglePacket = reinterpret_cast<ViewingAnglePacket_CtoS*>(pOverlappedEx->buffer);

                client->angle_x = viewAnglePacket->x;
                client->angle_y = viewAnglePacket->y;
                client->angle_z = viewAnglePacket->z;
                std::cout << "[�ü�] Ŭ���̾�Ʈ " << client->id
                    << " �ü�����: (" << client->angle_x << ", " << client->angle_y << ", " << client->angle_z << ")\n";

                // �̵� ��Ŷ�� ��� Ŭ���̾�Ʈ���� ����
                for (stClientInfo* otherClient : clients) {
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // ��Ŷ�� ���� Ŭ���̾�Ʈ���Դ� �ٽ� �������� ����
                        SendData_ViewAngle(client, otherClient);
                    }
                }
            }
            else if (*packetType == PacketType::CHAT) {
                //if (bytesTransferred < sizeof(ChatPacket)) {
                //    std::cerr << "[����] CHAT ��Ŷ ũ�� ����: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                ChatPacket_CtoS* chatPacket = reinterpret_cast<ChatPacket_CtoS*>(pOverlappedEx->buffer);
                std::string msg{ chatPacket->message,bytesTransferred - sizeof(PacketType) };

                std::cout << "[ä��] Ŭ���̾�Ʈ " << client->id << ": " << msg << std::endl;

                //ä�� ��Ŷ�� ��� Ŭ���̾�Ʈ���� ����
                for (stClientInfo* otherClient : clients) {
                    if (otherClient != client) { // ��Ŷ�� ���� Ŭ���̾�Ʈ���Դ� �ٽ� �������� ����
                        SendData(client, otherClient, msg.c_str(), bytesTransferred);
                        std::cout << "send\n";
                    }
                }
            }

            RegisterRecv(client);  // �ٽ� ���� ���
        }
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
