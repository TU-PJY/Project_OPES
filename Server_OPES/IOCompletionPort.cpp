#include "IOCompletionPort.h"
//#include"Packet.h"
#include <iostream>
#include <mswsock.h> // AcceptEx
#include <DirectXMath.h>
#pragma comment(lib, "Mswsock.lib")

using namespace DirectX;

// 맵 데이터를 읽기 위한 스크립트 객체
ScriptUtil terrainScript;

// 현재 로드된 터레인 정점 벡터
std::vector<XMFLOAT3> terrainData;

// 몬스터 생성 데이터를 읽기 위한 스크립트 객체
ScriptUtil monsterDataScript;

// 현재 로드된 몬스터 생성 타입 및 위치
std::vector<MonsterData> monsterData;

std::thread npcThread;


extern LPFN_ACCEPTEX lpfnAcceptEx = nullptr; // 전역으로 AcceptEx 포인터
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

    // AcceptEx 가져오기
    GUID guidAcceptEx = WSAID_ACCEPTEX;
    DWORD bytes;
    WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx, sizeof(guidAcceptEx), &lpfnAcceptEx, sizeof(lpfnAcceptEx), &bytes, NULL, NULL);

    std::cout << "소켓 초기화 성공\n";
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

    std::cout << "서버 등록 성공..\n";
    return true;
}
void IOCompletionPort::PostAccept() {
    stOverlappedEx* overlappedEx = new stOverlappedEx();
    overlappedEx->operation = IOOperation::ACCEPT;
    ZeroMemory(&overlappedEx->overlapped, sizeof(overlappedEx->overlapped));
    overlappedEx->acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    DWORD bytesReceived = 0;
    int addrLen = sizeof(SOCKADDR_IN) + 16;

    BOOL ret = lpfnAcceptEx(
        listenSocket,
        overlappedEx->acceptSocket,
        overlappedEx->buffer,
        0,
        addrLen,
        addrLen,
        &bytesReceived,
        &overlappedEx->overlapped
    );

    if (ret == FALSE && WSAGetLastError() != ERROR_IO_PENDING) {
        std::cerr << "AcceptEx 실패: " << WSAGetLastError() << "\n";
        delete overlappedEx;
    }
}

bool IOCompletionPort::StartServer() {
    iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 4);
    if (iocpHandle == NULL) {
        std::cerr << "[에러] CreateIoCompletionPort() 실패: " << GetLastError() << "\n";
        return false;
    }


    // 맵 터레인 데이터 로드
    terrainScript.Release();
    terrainData.clear();

    terrainScript.Load("mapData//terrain_map1.xml");
    auto LoadBehavior = [&](CategoryPtr Category)
        {
            XMFLOAT3 loadedVertex{};
            loadedVertex.x = terrainScript.LoadDigitData(Category, "x");
            loadedVertex.y = terrainScript.LoadDigitData(Category, "y");
            loadedVertex.z = terrainScript.LoadDigitData(Category, "z");
            terrainData.emplace_back(loadedVertex);
        };
    terrainScript.LoadAllData(LoadBehavior);


    // 몬스터 타입 및 생성 위치 로드
    monsterDataScript.Release();
    monsterData.clear();

    monsterDataScript.Load("mapData//monster_map1.xml");
    unsigned int monsterIdCounter = 0;
    auto LoadMonsterData = [&](CategoryPtr Category)
        {
            MonsterData loadedData{};
            loadedData.monsterType = (int)monsterDataScript.LoadDigitData(Category, "type");
            loadedData.createPointX = monsterDataScript.LoadDigitData(Category, "x");
            loadedData.createPointZ = monsterDataScript.LoadDigitData(Category, "z");
            loadedData.hp = 100; // 기본 HP 할당
            loadedData.id = monsterIdCounter++; // 고유 ID 부여
            monsterData.emplace_back(loadedData);
        };
    monsterDataScript.LoadAllData(LoadMonsterData);

    CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, 9999, 0);
    PostAccept();
    workerThread = std::thread([this]() { WorkThread(); });
    npcThread = std::thread([this]() { NPCAIThread(); });
    //npcThread = std::thread([this]() {
    //    try {
    //        NPCAIThread();
    //    }
    //    catch (const std::exception& e) {
    //        std::cerr << "[NPCThread 예외] " << e.what() << "\n";
    //    }
    //    });
    std::cout << "서버가 시작되었습니다.\n";
    return true;
}
void IOCompletionPort::SendData_MonsterMoveToAllClients(const MonsterData& m) {
    
    for (auto* client : clients) {
        if (!client || client->alreadyRemoved || client->socketClient == INVALID_SOCKET)
            continue;
        MovePacket_StoC* packet = new MovePacket_StoC{};
        packet->type = PacketType::MONSTER_MOVE;  // 필요시 MONSTER_MOVE로 별도 정의 가능
        packet->id = m.id;
        packet->x = m.createPointX;
        packet->y = 0.0f;  // 고정값 혹은 높이 정보가 있다면 그 값으로
        packet->z = m.createPointZ;
        stOverlappedEx* sendOver = new stOverlappedEx{};
        ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
        sendOver->operation = IOOperation::SEND;
        sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
        sendOver->wsaBuf.len = sizeof(MovePacket_StoC);
        sendOver->cleanup = [packet, sendOver]() {
           // std::cout << "[디버그] cleanup called for monster move packet\n";
            delete packet;
            delete sendOver;
         };

        int ret = WSASend(client->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
            &sendOver->overlapped, NULL);

        //if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        //    std::cerr << "[에러] WSASend 실패(MonsterMove): " << WSAGetLastError() << std::endl;
        //    closesocket(client->socketClient);
        //    RemoveClient(client);
        //    delete packet;
        //    delete sendOver;
        //}
        if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            std::cerr << "[에러] WSASend 실패(MonsterMove): " << WSAGetLastError() << std::endl;

            if (!client->alreadyRemoved) {
                closesocket(client->socketClient);
                RemoveClient(client);
            }

            // cleanup 람다 등록했으므로 delete 중복 금지
            // delete packet;
            // delete sendOver;
        }
    }
}

void IOCompletionPort::NPCAIThread() {
    const float speed = 0.05f;
    const int frameTimeMs = 60;
    //SendData_MonsterMoveToAllClients(monsterData[0]);
    while (isRunning) {
        auto frameStart = std::chrono::steady_clock::now();

        for (auto& m : monsterData) {
            // 조건: 스콜피온이면서 추적 상태
            if (m.monsterType == 2 && m.state == 1) {
                stClientInfo* target = nullptr;

                for (auto* c : clients) {
                    if (!c) continue;
                    if (c->id == m.targetClientId) {
                        target = c;
                        break;
                    }
                }

                if (target) {
                    float dx = target->x - m.createPointX;
                    float dz = target->z - m.createPointZ;
                    float dist = std::sqrt(dx * dx + dz * dz);

                    if (dist > 1e-3f) {
                        m.createPointX += dx / dist * speed;
                        m.createPointZ += dz / dist * speed;
                        SendData_MonsterMoveToAllClients(m);
                       // std::cout << "[디버그] SendData_MonsterMoveToAllClients called for monsterID: "
                       //     << m.id << " at (" << m.createPointX << "," << m.createPointZ << ")\n";
                 
                    }
                }
            }
        }

        auto frameEnd = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();
        if (duration < frameTimeMs)
            std::this_thread::sleep_for(std::chrono::milliseconds(frameTimeMs - duration));
    }
}
//void IOCompletionPort::AcceptThread() {
//    while (isRunning) {
//        SOCKADDR_IN clientAddr;
//        int addrLen = sizeof(SOCKADDR_IN);
//
//        // 클라이언트 구조체를 동적 할당하여 저장
//        stClientInfo* newClient = new stClientInfo();
//        newClient->socketClient = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
//        if (newClient->socketClient == INVALID_SOCKET) {
//            std::cerr << "[에러] accept() 실패\n";
//            delete newClient;  // 메모리 해제
//            continue;
//        }
//
//        // NAGLE 비활성화
//        BOOL bNoDelay = TRUE;
//        int result = setsockopt(newClient->socketClient, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(BOOL));
//        if (result == SOCKET_ERROR) {
//            std::cerr << "[에러] setsockopt(TCP_NODELAY) 실패: " << WSAGetLastError() << std::endl;
//        }
//        ///
//
//        CreateIoCompletionPort((HANDLE)newClient->socketClient, iocpHandle, (ULONG_PTR)newClient, 0);
//
//        idCount++;
//        newClient->id = idCount;
//        clients.push_back(newClient);
//        NotifyOthersAboutNewClient(newClient);
//        SendExistingClientsToNewClient(newClient);
//        std::cout << "새로운 클라이언트 접속! 현재 클라이언트 수: " << clients.size() << "\n";
//        //
//        std::lock_guard<std::mutex> lock(waitMutex);
//        waitingClients.push_back(newClient);
//        if (waitingClients.size() >= 3) {
//            std::vector<stClientInfo*> roomMembers(waitingClients.begin(), waitingClients.begin() + 3);
//            waitingClients.erase(waitingClients.begin(), waitingClients.begin() + 3);
//
//            CreateRoom(roomMembers);
//            for (auto& client : roomMembers) {
//                SendData_EnterRoom(client);
//                RegisterRecv(client);
//            }
//        }
//        else {
//            SendData_EnterRoom(newClient);
//            RegisterRecv(newClient);
//        }
//
//
//    }
//}
//void IOCompletionPort::SendExistingClientsToNewClient(stClientInfo* newClient) {
//    ExistingClientsDataPacket packet;
//    packet.type = PacketType::EXISTING_CLIENTS;
//    packet.count = 0;
//
//    for (auto& client : clients) {
//        if (!client) continue;
//        if (client != newClient && client->roomID == newClient->roomID) {
//            auto& dst = packet.clients[packet.count++];
//            dst.id = client->id;
//            dst.x = client->x;
//            dst.y = client->y;
//            dst.z = client->z;
//            dst.angle_x = client->angle_x;
//            dst.angle_y = client->angle_y;
//            dst.angle_z = client->angle_z;
//        }
//    }
//
//    if (packet.count == 0)
//        return;
//
//    newClient->sendOverlapped.operation = IOOperation::SEND;
//    ZeroMemory(&newClient->sendOverlapped.overlapped, sizeof(newClient->sendOverlapped.overlapped));
//    newClient->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&packet);
//    newClient->sendOverlapped.wsaBuf.len = sizeof(PacketType) + sizeof(unsigned int) + packet.count * sizeof(packet.clients[0]);
//
//    DWORD size_sent = 0;
//    int ret = WSASend(newClient->socketClient, &newClient->sendOverlapped.wsaBuf, 1, &size_sent, 0, &newClient->sendOverlapped.overlapped, NULL);
//    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
//        std::cerr << "[에러] WSASend(EXISTING_CLIENTS) 실패: " << WSAGetLastError() << std::endl;
//        closesocket(newClient->socketClient);
//        RemoveClient(newClient);
//    }
//}
//void IOCompletionPort::NotifyOthersAboutNewClient(stClientInfo* newClient) {
//    NewClientPacket pkt;
//    pkt.type = PacketType::NEW_CLIENT;
//    pkt.id = newClient->id;
//
//    for (auto& other : clients) {
//        if (!other) continue;
//        if (other != newClient && other->roomID == newClient->roomID) {
//            other->sendOverlapped.operation = IOOperation::SEND;
//            ZeroMemory(&other->sendOverlapped.overlapped, sizeof(other->sendOverlapped.overlapped));
//            other->sendOverlapped.wsaBuf.buf = reinterpret_cast<char*>(&pkt);
//            other->sendOverlapped.wsaBuf.len = sizeof(pkt);
//
//            DWORD sent = 0;
//            int ret = WSASend(other->socketClient, &other->sendOverlapped.wsaBuf, 1, &sent, 0, &other->sendOverlapped.overlapped, NULL);
//            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
//                std::cerr << "[에러] WSASend 실패 (NEW_CLIENT): " << WSAGetLastError() << "\n";
//                closesocket(other->socketClient);
//                RemoveClient(other);
//            }
//        }
//    }
//}
void IOCompletionPort::CreateRoom(const std::vector<stClientInfo*>& members) {
    //std::lock_guard<std::mutex> lock(roomMutex);

    Room newRoom;
    nextRoomID++;
    newRoom.roomID = nextRoomID;
    newRoom.clients = members;
    //for (const auto& m : monsterData) {
    //    MonsterData copy = m;
    //    newRoom.monsters.push_back(copy);
    //}
    //->이거에 대한 접근 예시
    //auto& room = rooms[2];
    //if (!room.monsters.empty()) {
    //    std::cout << "HP: " << room.monsters[0].hp << std::endl;
    //}
    for (auto* client : members) {
        client->roomID = newRoom.roomID;
        // 방 입장 메시지 전송
        //SendRoomEnterMessage(client, newRoom.roomID);
    }

    rooms[newRoom.roomID] = newRoom;
    std::cout << "create room!: " << newRoom.roomID << std::endl;
}

void IOCompletionPort::SendData_EnterRoom(stClientInfo* receiver) {
    EnterRoomPacket* packet = new EnterRoomPacket{};
    packet->type = PacketType::ENTER;
    packet->roomID = receiver->roomID;
    packet->myID = receiver->id;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(EnterRoomPacket);
    sendOver->cleanup = [packet, sendOver]() {delete packet; delete sendOver; };
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}
void IOCompletionPort::RegisterRecv(stClientInfo* client) {
    if (!client || client->socketClient == INVALID_SOCKET) {
        std::cerr << "[에러] 유효하지 않은 클라이언트 소켓\n";
        return;
    }

    DWORD flags = 0;
    DWORD bytesReceived = 0;
    client->recvOverlapped.operation = IOOperation::RECV;
    client->recvOverlapped.wsaBuf.len = MAX_SOCKBUF;
    client->recvOverlapped.wsaBuf.buf = client->recvOverlapped.buffer;

    int result = WSARecv(client->socketClient, &client->recvOverlapped.wsaBuf, 1, &bytesReceived, &flags, &client->recvOverlapped.overlapped, NULL);
    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[에러] WSARecv 실패: " << WSAGetLastError() << std::endl;
        closesocket(client->socketClient);
        RemoveClient(client);
    }
}
bool IOCompletionPort::AddClient(stClientInfo* c)
{
    for (auto& slot : clients)
        if (slot == nullptr) { slot = c; ++clientCount; return true; }
    std::cerr << "[경고] MAX_CLIENTS 초과\n";
    return false;
}

void IOCompletionPort::RemoveClient(stClientInfo* c)
{
    if (!c) return;

    // ① 이미 제거 처리했다면 아무 것도 하지 않는다
    if (c->alreadyRemoved.exchange(true))
        return;

    // ② 소켓이 열려 있으면 지금 닫는다
    if (c->socketClient != INVALID_SOCKET) {
        shutdown(c->socketClient, SD_BOTH);
        closesocket(c->socketClient);
        c->socketClient = INVALID_SOCKET;
    }

    for (auto& slot : clients)
        if (slot == c) { slot = nullptr; --clientCount; break; }

    waitingClients.erase(
        std::remove(waitingClients.begin(), waitingClients.end(), c),
        waitingClients.end());
    delete c;
}






void IOCompletionPort::SendData_Move(stClientInfo* sender, stClientInfo* receiver) {
    MovePacket_StoC* packet = new MovePacket_StoC{};
    packet->type = PacketType::MOVE;
    packet->id = sender->id;
    packet->x = sender->x;
    packet->y = sender->y;
    packet->z = sender->z;

    stOverlappedEx* sendOver = new stOverlappedEx();
    ZeroMemory(&sendOver->overlapped, sizeof(WSAOVERLAPPED));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(MovePacket_StoC);
    sendOver->cleanup = [packet, sendOver]() {
        delete packet;
        delete sendOver;
        };
    DWORD sizeSent = 0;
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, &sizeSent, 0,
        &sendOver->overlapped,
        NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[에러] WSASend 실패(MOVE): " << WSAGetLastError() << std::endl;
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}


void IOCompletionPort::SendData_ViewAngle(stClientInfo* sender, stClientInfo* receiver) {
    ViewingAnglePacket_StoC* packet = new ViewingAnglePacket_StoC{};
    packet->type = PacketType::VIEW_ANGLE;
    packet->id = sender->id;
    packet->x = sender->angle_x;
    packet->y = sender->angle_y;
    packet->z = sender->angle_z;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(ViewingAnglePacket_StoC);

    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,
        NULL);
    sendOver->cleanup = [packet, sendOver]() {
        delete packet;
        delete sendOver;
        };
    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}

void IOCompletionPort::SendData_Animaion(stClientInfo* sender, stClientInfo* receiver) {
    AnimationPacket_StoC* packet = new AnimationPacket_StoC{};
    packet->type = PacketType::ANIMATION;
    packet->id = sender->id;
    packet->animationType = sender->animationType;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(AnimationPacket_StoC);
    sendOver->cleanup = [packet, sendOver]() {
        delete packet;
        delete sendOver;
        };
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,
        NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}

void IOCompletionPort::SendData_Player2Monster(unsigned int monsterID, unsigned int damage, stClientInfo* receiver) {
    Player2Monster* packet = new Player2Monster{};
    packet->type = PacketType::PLAYER_TO_MOSTER;
    packet->monsterId = monsterID;
    packet->damage = damage;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(Player2Monster);
    sendOver->cleanup = [packet, sendOver]() {
        delete packet;
        delete sendOver;
        };
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,
        NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}

//void IOCompletionPort::SendData_EnterRoom(stClientInfo* receiver) {
//    EnterRoomPacket* packet = new EnterRoomPacket{};
//    packet->type = PacketType::ENTER;
//    packet->roomID = receiver->roomID;
//    packet->myID = receiver->id;
//
//    stOverlappedEx* sendOver = new stOverlappedEx{};
//    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
//    sendOver->operation = IOOperation::SEND;
//    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
//    sendOver->wsaBuf.len = sizeof(EnterRoomPacket);
//
//    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
//        &sendOver->overlapped,
//        [](DWORD, DWORD, LPWSAOVERLAPPED overlapped, DWORD) {
//            auto* ex = reinterpret_cast<stOverlappedEx*>(overlapped);
//            delete reinterpret_cast<EnterRoomPacket*>(ex->wsaBuf.buf);
//            delete ex;
//        });
//
//    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
//        closesocket(receiver->socketClient);
//        RemoveClient(receiver);
//        delete packet;
//        delete sendOver;
//    }
//}

void IOCompletionPort::SendData(stClientInfo* sender, stClientInfo* receiver, const char* message, int length) {
    ChatPacket_StoC* packet = new ChatPacket_StoC{};
    packet->type = PacketType::CHAT;
    packet->id = sender->id;
    memcpy(packet->message, message, length - sizeof(PacketType));

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = length + sizeof(unsigned int);
    sendOver->cleanup = [packet, sendOver]() {
        delete packet;
        delete sendOver;
        };
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,
        NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}

void IOCompletionPort::SendExistingClientsToNewClient(stClientInfo* receiver) {
    ExistingClientsDataPacket* packet = new ExistingClientsDataPacket{};
    packet->type = PacketType::EXISTING_CLIENTS;
    packet->count = 0;
    for (auto& client : clients) {
        if (!client || client == receiver || client->roomID != receiver->roomID) continue;
        auto& dst = packet->clients[packet->count++];
        dst.id = client->id;
        dst.x = client->x;
        dst.y = client->y;
        dst.z = client->z;
        dst.angle_x = client->angle_x;
        dst.angle_y = client->angle_y;
        dst.angle_z = client->angle_z;
    }

    if (packet->count == 0) { delete packet; return; }

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(PacketType) + sizeof(unsigned int) + packet->count * sizeof(packet->clients[0]);
    sendOver->cleanup = [packet, sendOver]() {
        delete packet;
        delete sendOver;
        };
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,
        NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        closesocket(receiver->socketClient);
        RemoveClient(receiver);
        delete packet;
        delete sendOver;
    }
}

void IOCompletionPort::NotifyOthersAboutNewClient(stClientInfo* newClient) {
    for (auto& other : clients) {
        if (!other || other == newClient || other->roomID != newClient->roomID) continue;

        NewClientPacket* pkt = new NewClientPacket{};
        pkt->type = PacketType::NEW_CLIENT;
        pkt->id = newClient->id;

        stOverlappedEx* sendOver = new stOverlappedEx{};
        ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
        sendOver->operation = IOOperation::SEND;
        sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
        sendOver->wsaBuf.len = sizeof(NewClientPacket);
        sendOver->cleanup = [pkt, sendOver]() {
            delete pkt;
            delete sendOver;
            };
        int ret = WSASend(other->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
            &sendOver->overlapped,
            NULL);

        if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            closesocket(other->socketClient);
            RemoveClient(other);
            delete pkt;
            delete sendOver;
        }
    }
}
void IOCompletionPort::SendData_MonsterState(stClientInfo* receiver,unsigned int monsterType, unsigned int monsterState, unsigned int id) {
        MonsterStatePacket_StoC* pkt = new MonsterStatePacket_StoC{};
        pkt->Ptype = PacketType::MONSTER_STATE;
        //pkt->Mtype = monsterType;
        pkt->state = monsterState;
        pkt->id = id;

        stOverlappedEx* sendOver = new stOverlappedEx{};
        ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
        sendOver->operation = IOOperation::SEND;
        sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
        sendOver->wsaBuf.len = sizeof(MonsterStatePacket_StoC);
        sendOver->cleanup = [pkt, sendOver]() {
            delete pkt;
            delete sendOver;
            };
        int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
            &sendOver->overlapped,
            NULL);

        if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            closesocket(receiver->socketClient);
            RemoveClient(receiver);
            delete pkt;
            delete sendOver;
        }
    
}

void IOCompletionPort::WorkThread() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;

    while (isRunning) {
        BOOL result = GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &completionKey, &overlapped, INFINITE);
        stOverlappedEx* pOverlappedEx = reinterpret_cast<stOverlappedEx*>(overlapped);
        if (pOverlappedEx->operation == IOOperation::ACCEPT) {
            stClientInfo* newClient = new stClientInfo();
            newClient->socketClient = pOverlappedEx->acceptSocket;

            setsockopt(newClient->socketClient, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                (char*)&listenSocket, sizeof(listenSocket));
            // 소켓 옵션 설정
            BOOL bNoDelay = TRUE;
            setsockopt(newClient->socketClient, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(BOOL));

            CreateIoCompletionPort((HANDLE)newClient->socketClient, iocpHandle, (ULONG_PTR)newClient, 0);
            idCount++;
            newClient->id = idCount;
            //clients.push_back(newClient);
            AddClient(newClient);
            // 기존 처리 함수 호출
            
            std::cout << "입장:" << idCount << std::endl;
            {
                std::lock_guard<std::mutex> lock(waitMutex);
                waitingClients.push_back(newClient);
                if (waitingClients.size() >= 3) {
                    std::vector<stClientInfo*> roomMembers(waitingClients.begin(), waitingClients.begin() + 3);
                    waitingClients.erase(waitingClients.begin(), waitingClients.begin() + 3);
                    CreateRoom(roomMembers);
                    for (auto& client : roomMembers) {
                        SendData_EnterRoom(client);  // 여기서 먼저 룸 ID를 클라이언트에 전송
                        NotifyOthersAboutNewClient(client);
                        SendExistingClientsToNewClient(client);
                        RegisterRecv(client);
                    }
                }
                else {
                    SendData_EnterRoom(newClient);  // 대기 중인 상태 전송 (roomID == 0)
                    NotifyOthersAboutNewClient(newClient);
                    SendExistingClientsToNewClient(newClient);
                    RegisterRecv(newClient);
                }
            }

            // 다음 AcceptEx 등록
            PostAccept();
            delete pOverlappedEx;
            continue;
        }
        
        // -------------- [DISCONNECT 처리] --------------
        if (!result || bytesTransferred == 0)
        {
            if (completionKey == 9999 || completionKey == 0) {
                // 리스닝 소켓용 AcceptEx 실패·취소
                delete pOverlappedEx;          // <- 이때는 Accept overlapped라 delete OK
                PostAccept();
                continue;
            }

            stClientInfo* client = reinterpret_cast<stClientInfo*>(completionKey);

            bool found = std::any_of(clients.begin(), clients.end(),
                [&](auto* s) { return s == client; });
            if (!found)
            {
                // 이미 삭제된 포인터이거나 쓰레기 포인터 -> overlapped만 Accept일 가능성
                if (pOverlappedEx && pOverlappedEx->operation == IOOperation::ACCEPT)
                    delete pOverlappedEx;
                continue;
            }

            std::cerr << "[연결 종료] 클라이언트 ID "
                << client->id << " 접속 해제\n";

            RemoveClient(client);              // client와 함께 overlapped 메모리도 해제됨
            // !! 여기서 pOverlappedEx 는 client 내부 메모리 → 더 이상 delete 금지 !!
            continue;
        }
        // ----------------------------------------------




       
        if (pOverlappedEx->operation == IOOperation::RECV) {
            stClientInfo* client = reinterpret_cast<stClientInfo*>(completionKey);

            PacketType* packetType = reinterpret_cast<PacketType*>(pOverlappedEx->buffer);

           // std::cout << "[디버깅] 수신된 패킷 타입: " << static_cast<int>(*packetType)
           //     << ", 받은 바이트: " << bytesTransferred << std::endl;

            if (*packetType == PacketType::MOVE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                MovePacket_CtoS* movePacket = reinterpret_cast<MovePacket_CtoS*>(pOverlappedEx->buffer);

                // 이동 처리
               
                client->x = movePacket->x;
                client->y = movePacket->y;
                client->z = movePacket->z;
               // std::cout << "[이동] 클라이언트 " << client->id
                //    << " 위치: (" << client->x << ", " << client->y << ", " << client->z<<")\n";

                // 이동 패킷을 모든 클라이언트에게 전송
                for (stClientInfo* otherClient : clients) {
                    if (!otherClient) continue;
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                        SendData_Move(client, otherClient);
                    }
                }
            }
            else if (*packetType == PacketType::VIEW_ANGLE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                ViewingAnglePacket_CtoS* viewAnglePacket = reinterpret_cast<ViewingAnglePacket_CtoS*>(pOverlappedEx->buffer);

                client->angle_x = viewAnglePacket->x;
                client->angle_y = viewAnglePacket->y;
                client->angle_z = viewAnglePacket->z;
                //std::cout << "[시선] 클라이언트 " << client->id
                //    << " 시선각도: (" << client->angle_x << ", " << client->angle_y << ", " << client->angle_z << ")\n";

                // 이동 패킷을 모든 클라이언트에게 전송
                for (stClientInfo* otherClient : clients) {
                    if (!otherClient) continue;
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                        SendData_ViewAngle(client, otherClient);
                    }
                }
            }
            else if (*packetType == PacketType::ANIMATION) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                AnimationPacket_CtoS* anyPacket = reinterpret_cast<AnimationPacket_CtoS*>(pOverlappedEx->buffer);

                client->animationType = anyPacket->anymationType;
                
                //std::cout << "[애니매이션] 클라이언트 " << client->animationType << " \n";

                // 이동 패킷을 모든 클라이언트에게 전송
                for (stClientInfo* otherClient : clients) {
                    if (!otherClient) continue;
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                        SendData_Animaion(client, otherClient);
                    }
                }
            }
            else if (*packetType == PacketType::MONSTER_STATE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                MonsterStatePacket_CtoS* pkt = reinterpret_cast<MonsterStatePacket_CtoS*>(pOverlappedEx->buffer);
                std::cout <<"Monstertype:" << pkt->Mtype <<", state:"<< pkt->state << std::endl;

                // 몬스터 상태 저장
                for (auto& m : monsterData) {
                    if (m.id == pkt->id) {
                        m.state = pkt->state;

                        if (pkt->state == 1 && pkt->Mtype == 2) {
                            m.targetClientId = client->id;  // 해당 클라이언트를 추적 대상으로 설정
                        }
                        else {
                            m.targetClientId = -1;  // 추적 멈춤
                        }

                        break;
                    }
                }
                // 데미지 패킷을 모든 클라이언트에게 전송
                for (stClientInfo* otherClient : clients) {
                    if (!otherClient) continue;
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                        SendData_MonsterState(otherClient,pkt->Mtype, pkt->state, pkt->id);
                    }
                }
            }
            else if (*packetType == PacketType::PLAYER_TO_MOSTER) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                Player2Monster* damagePacket = reinterpret_cast<Player2Monster*>(pOverlappedEx->buffer);

                // 데미지 패킷을 모든 클라이언트에게 전송
                for (stClientInfo* otherClient : clients) {
                    if (!otherClient) continue;
                    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                        SendData_Player2Monster(damagePacket->monsterId, damagePacket->damage, otherClient);

                    }
                }
            }
            else if (*packetType == PacketType::CHAT) {
                //if (bytesTransferred < sizeof(ChatPacket)) {
                //    std::cerr << "[에러] CHAT 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                ChatPacket_CtoS* chatPacket = reinterpret_cast<ChatPacket_CtoS*>(pOverlappedEx->buffer);
                std::string msg{ chatPacket->message,bytesTransferred - sizeof(PacketType) };

                std::cout << "[채팅] 클라이언트 " << client->id << ": " << msg << std::endl;

                //채팅 패킷을 모든 클라이언트에게 전송
                for (stClientInfo* otherClient : clients) {
                    if (!otherClient) continue;
                    if (otherClient != client) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                        SendData(client, otherClient, msg.c_str(), bytesTransferred);
                        std::cout << "send\n";
                    }
                }
            }

            RegisterRecv(client);  // 다시 수신 대기
        }
        if (pOverlappedEx->operation == IOOperation::SEND) {
            if (pOverlappedEx->cleanup) {
                pOverlappedEx->cleanup();  // 💡 안전하게 패킷 + overlapped 메모리 해제
            }
            else {
                delete pOverlappedEx;
            }
            continue;
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
   // if (npcThread.joinable()) npcThread.join();

    std::cout << "서버 종료 완료.\n";
}
