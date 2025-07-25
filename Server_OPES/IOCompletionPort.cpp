#include "IOCompletionPort.h"
//#include"Packet.h"
#include "Math.h"
#include <iostream>
#include <mswsock.h> // AcceptEx
#include <DirectXMath.h>
#include<random>
#include<cmath>
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

std::mutex roomMapMutex;
//std::vector<MonsterData> myMonsters;//임시

//std::vector<MonsterData> defenseMonsters(DEFENSE_MONSTER);//임시

//std::mutex defenseMonsterMutex;//임시

//bool defenseState = true;//임시

//int clearCount = 0;//임시


//int roomHp= CENTER_HP;//임시


// NPC 스레드 프레임 시간
float deltaTime{};

bool randomTreadFlag = true;
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
            loadedData.id = monsterIdCounter++; // 고유 ID 부여
            monsterData.emplace_back(loadedData);
            //myMonsters.emplace_back(loadedData);//임시데이터
        };
    monsterDataScript.LoadAllData(LoadMonsterData);

    CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, 9999, 0);
    PostAccept();
    workerThread = std::thread([this]() { WorkThread(); });
   
    
    std::cout << "서버가 시작되었습니다.\n";
    return true;
}
std::random_device RD;
XMFLOAT2 GenPointInDonut(float DiameterMin, float DiameterMax, const XMFLOAT2& Center) {
    static std::default_random_engine dre(RD());
    static std::uniform_real_distribution<float> DistAngle(0.0f, 2.0f * XM_PI);
    static std::uniform_real_distribution<float> DistRadius(0.0f, 1.0f);

    float Angle = DistAngle(dre);
    float RadiusMin = DiameterMin * 0.5;
    float RadiusMax = DiameterMax * 0.5;
    float R = std::sqrt((RadiusMax * RadiusMax - RadiusMin * RadiusMin) * DistRadius(dre) + RadiusMin * RadiusMin);

    float X = R * std::cosf(Angle) + Center.x;
    float Y = R * std::sinf(Angle) + Center.y;
    return XMFLOAT2(X, Y);
    //GenPointInDonut(30.0,60.0,XMFLOAT2(-120.0,-120.0));
}
///void IOCompletionPort::RandomPositionThread() {
///    using namespace std::chrono;
///    int monsterIdCount = 0;
///    while (defenseState) {
///        std::this_thread::sleep_for(std::chrono::seconds(2));
///        XMFLOAT2 rendomPosition = GenPointInDonut(30.0, 60.0, XMFLOAT2(-120.0, -120.0));
///
///        for (auto* client : clients) {
///            if (!client || client->alreadyRemoved || client->socketClient == INVALID_SOCKET)
///                continue;
///            DefenseRandomPacket* packet = new DefenseRandomPacket{};
///            packet->type = PacketType::RANDOM_POSITION;
///            packet->monsterID = monsterIdCount; // 특별한 ID 
///            packet->x = rendomPosition.x;
///            packet->z = rendomPosition.y; 
///            {
///                std::lock_guard<std::mutex> lock(defenseMonsterMutex);
///                defenseMonsters[monsterIdCount].createPointX = rendomPosition.x;
///                defenseMonsters[monsterIdCount].createPointZ = rendomPosition.y;
///                defenseMonsters[monsterIdCount].id = monsterIdCount;
///            }
///            stOverlappedEx* sendOver = new stOverlappedEx{};
///            ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
///            sendOver->operation = IOOperation::SEND;
///            sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
///            sendOver->wsaBuf.len = sizeof(DefenseRandomPacket);
///            sendOver->cleanup = [packet, sendOver]() {
///                delete packet;
///                delete sendOver;
///            };
///
///            int ret = WSASend(client->socketClient, &sendOver->wsaBuf, 1, nullptr, 0, &sendOver->overlapped, NULL);
///            if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
///                std::cerr << "[에러] 랜덤 위치 전송 실패: " << WSAGetLastError() << std::endl;
///                RemoveClient(client);
///            }
///        }
///        monsterIdCount++;
///        if (monsterIdCount == DEFENSE_MONSTER) {
///            std::cout << "[랜덤 위치 전송] 20개 완료 → 쓰레드 종료됨\n";
///            break;
///        }
///       // std::this_thread::sleep_for(std::chrono::seconds(2));
///    }
///}
void IOCompletionPort::RandomPositionThread() {
    using namespace std::chrono;

    while (isRunning) {
        
        {
            std::lock_guard<std::mutex> lock(roomMapMutex);
            for (auto& [roomID, room] : rooms) {
                if (!room.isCreat)
                    continue;

                for (int monsterId = 0; monsterId < DEFENSE_MONSTER; ++monsterId) {

                    std::this_thread::sleep_for(std::chrono::seconds(2));

                    for (auto* client : room.clients) {
                        if (!client || client->alreadyRemoved || client->socketClient == INVALID_SOCKET)
                            continue;

                        DefenseRandomPacket* packet = new DefenseRandomPacket{};
                        packet->type = PacketType::RANDOM_POSITION;
                        packet->monsterID = monsterId;
                        packet->x = room.defenseMonsters[monsterId].createPointX;
                        packet->z = room.defenseMonsters[monsterId].createPointZ;

                        stOverlappedEx* sendOver = new stOverlappedEx{};
                        ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
                        sendOver->operation = IOOperation::SEND;
                        sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
                        sendOver->wsaBuf.len = sizeof(DefenseRandomPacket);
                        sendOver->cleanup = [packet, sendOver]() {
                            delete packet;
                            delete sendOver;
                            };

                        int ret = WSASend(client->socketClient, &sendOver->wsaBuf, 1, nullptr, 0, &sendOver->overlapped, NULL);
                        if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                            std::cerr << "[에러] 랜덤 위치 전송 실패 (room " << roomID << "): " << WSAGetLastError() << std::endl;
                            RemoveClient(client);
                        }
                    }

                }

                room.isCreat = false;
                std::cout << "[Room " << roomID << "] 랜덤 위치 20개 전송 완료\n";
            }
        }
    }
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



void IOCompletionPort::CreateRoom(const std::vector<stClientInfo*>& members) {
    //std::lock_guard<std::mutex> lock(roomMutex);

    Room newRoom;
    newRoom.roomID = nextRoomID;
    newRoom.clients = members;
    nextRoomID++;
    for (const auto& m : monsterData) {
        MonsterData copy = m;
        newRoom.myMonsters.push_back(copy);
    }
    //->이거에 대한 접근 예시
    //auto& room = rooms[2];
    //if (!room.monsters.empty()) {
    //    std::cout << "HP: " << room.monsters[0].hp << std::endl;
    //}

    for (int i = 0; i < DEFENSE_MONSTER; ++i) {
        MonsterData def;
        def.id = i;
        def.hp = 100;
        def.state = 0;
        XMFLOAT2 rendomPosition = GenPointInDonut(30.0, 60.0, XMFLOAT2(-120.0, -120.0));
        def.createPointX = rendomPosition.x;
        def.createPointZ = rendomPosition.y;
        newRoom.defenseMonsters[i] = def;
    }
    for (auto* client : members) {
        client->roomID = newRoom.roomID;
        // 방 입장 메시지 전송
        //SendRoomEnterMessage(client, newRoom.roomID);
    }

    // centerHp, clearCount, defenseState 기본 설정
    newRoom.centerHp = CENTER_HP;
    newRoom.clearCount = 0;
    newRoom.defenseState = true;
    //rooms[newRoom.roomID] = std::move(newRoom);
    {
        std::lock_guard<std::mutex> lock(roomMapMutex);
        rooms[newRoom.roomID] = std::move(newRoom);
    }
    std::cout << "create room!: " << newRoom.roomID << std::endl;

    if (randomTreadFlag ) {
        randomPositionThread = std::thread([this]() { RandomPositionThread(); });
        randomTreadFlag = false;
    }
}

void IOCompletionPort::SendData_EnterRoom(stClientInfo* receiver) {
    EnterRoomPacket* packet = new EnterRoomPacket{};
    packet->type = PacketType::ENTER;
    packet->roomID = receiver->roomID;
    packet->myID = receiver->id;
    std::cout << "SendData_EnterRoom ID:" << receiver->id << std::endl;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(packet);
    sendOver->wsaBuf.len = sizeof(EnterRoomPacket);
    sendOver->cleanup = [packet, sendOver]() {delete packet; delete sendOver; };
    int ret = WSASend(receiver->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
        &sendOver->overlapped,NULL);

    if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cout << "ERROR-SendData_EnterRoom to ID: " << receiver->id << " / room: " << receiver->roomID << std::endl;
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

    stOverlappedEx* recvOver = new stOverlappedEx{};
    ZeroMemory(recvOver, sizeof(stOverlappedEx));

    recvOver->operation = IOOperation::RECV;
    recvOver->wsaBuf.len = MAX_SOCKBUF;
    recvOver->wsaBuf.buf = recvOver->buffer;

    // ✅ cleanup 콜백 등록
    recvOver->cleanup = [recvOver]() {
        delete recvOver;
        };

    int result = WSARecv(
        client->socketClient,
        &recvOver->wsaBuf,
        1,
        &bytesReceived,
        &flags,
        &recvOver->overlapped,
        NULL
    );

    //std::cout << "[DEBUG] RegisterRecv 요청\n";

    if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        std::cerr << "[에러] WSARecv 실패: " << WSAGetLastError() << std::endl;
        closesocket(client->socketClient);
        RemoveClient(client);
        recvOver->cleanup();  // 실패 시 즉시 해제
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
    for (int i = 0; i < clientCount;i++) {
        if (!clients[i] || clients[i] == receiver /*|| *//*client->roomID != receiver->roomID*/) continue;

        auto& dst = packet->clients[i];
        packet->count++;
        dst.id = clients[i]->id;
        
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
    for (int i = 0; i < clientCount;i++) {
        if (!clients[i] || clients[i] == newClient/* ||*/ /*other->roomID != newClient->roomID*/) continue;

        NewClientPacket* pkt = new NewClientPacket{};
        pkt->type = PacketType::NEW_CLIENT;
        pkt->id = newClient->id;
        std::cout << "NotifyOthersAboutNewClient-id:" << pkt->id << std::endl;


        stOverlappedEx* sendOver = new stOverlappedEx{};
        ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
        sendOver->operation = IOOperation::SEND;
        sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
        sendOver->wsaBuf.len = sizeof(NewClientPacket);
        sendOver->cleanup = [pkt, sendOver]() {
            delete pkt;
            delete sendOver;
        };
        int ret = WSASend(clients[i]->socketClient, &sendOver->wsaBuf, 1, nullptr, 0,
            &sendOver->overlapped,
            NULL);

        if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            closesocket(clients[i]->socketClient);
            RemoveClient(clients[i]);
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
void IOCompletionPort::SendData_MonsterMove(stClientInfo* receiver,float x, float y, float z, float angle, unsigned int monsterId,unsigned int playerId) {
    MonsterMovePacket* pkt = new MonsterMovePacket{};
    pkt->type = PacketType::MONSTER_MOVE;
    pkt->x = x;
    pkt->y = y;
    pkt->z = z;
    pkt->angle_y= angle;
    pkt->playerId = playerId;
    pkt->monsterId = monsterId;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(MonsterMovePacket);
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
void IOCompletionPort::SendData_MtoPDamagePacket(stClientInfo* receiver, unsigned int playerID, unsigned int monsterID, int attackHp) {
    MtoPDamagePacket* pkt = new MtoPDamagePacket{};
    pkt->type = PacketType::MTOP_DAMAGE;
    pkt->playerID = playerID;
    pkt->monsterID = monsterID;
    pkt->attackHp = attackHp;
   

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(MtoPDamagePacket);
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

void IOCompletionPort::SendData_PtoMDamagePacket(stClientInfo* receiver,unsigned int monsterID,int attackHp) {
    PtoMDamagePacket* pkt = new PtoMDamagePacket{};
    pkt->type = PacketType::PTOM_DAMAGE;
    pkt->monsterID = monsterID;
    pkt->attackHp = attackHp;


    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(PtoMDamagePacket);
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
void IOCompletionPort::SendData_EngineerInstallPacket(stClientInfo* receiver, int type,unsigned int ID,float rotY,float posX,float posY,float posZ) {
    EngineerInstallPacket* pkt = new EngineerInstallPacket{};
    pkt->Ptype = PacketType::ENGINEER_INSTALL;
    pkt->Etype = type;
    pkt->ID = ID;
    pkt->posX = posX;
    pkt->posY = posY;
    pkt->posZ = posZ;
    pkt->rotY = rotY;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(EngineerInstallPacket);
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
void IOCompletionPort::SendData_EngineerObjectPacket(stClientInfo* receiver, unsigned int ID,int hp) {
    EngineerObjectPacket* pkt = new EngineerObjectPacket{};
    pkt->type = PacketType::ENGINEER_OBJECT;
    pkt->hp = hp;
    pkt->ID = ID;
    
    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(EngineerObjectPacket);
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
void IOCompletionPort::SendData_CenterBuildingPacket(stClientInfo* receiver, int hp) {
    CenterBuildingPacket* pkt = new CenterBuildingPacket{};
    pkt->type = PacketType::CENTER_HP;
    pkt->damage = hp;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(CenterBuildingPacket);
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
void IOCompletionPort::SendData_GrenadePacket(stClientInfo* receiver, float posX,float posY,float posZ,float rotX,float rotY,float rotZ) {
    GrenadePacket* pkt = new GrenadePacket{};
    pkt->type = PacketType::GRENADE;
    pkt->posX = posX;
    pkt->posY = posY;
    pkt->posZ = posZ;
    pkt->rotX = rotX;
    pkt->rotY = rotY;
    pkt->rotZ = rotZ;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(GrenadePacket);
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
void IOCompletionPort::SendData_PlayerArrivalPacket(stClientInfo* receiver, unsigned int playerID, bool arrive) {
    PlayerArrivalPacket* pkt = new PlayerArrivalPacket{};
    pkt->type = PacketType::PLAYER_ARRIVAL;
    pkt->playerID = playerID;
    pkt->arrive = arrive;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(PlayerArrivalPacket);
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
void IOCompletionPort::SendData_ClearCountPacket(stClientInfo* receiver, int PlayerCount) {
    ClearCountPacket* pkt = new ClearCountPacket{};
    pkt->type = PacketType::CLEAR_COUNT;
    pkt->PlayerCount = PlayerCount;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(ClearCountPacket);
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
void IOCompletionPort::SendData_ChooseJobPacket(stClientInfo* receiver, unsigned int playerID,int job) {
    ChooseJobPacket* pkt = new ChooseJobPacket{};
    pkt->type = PacketType::CHOOSE_JOB;
    pkt->playerID = playerID;
    pkt->job = job;

    stOverlappedEx* sendOver = new stOverlappedEx{};
    ZeroMemory(&sendOver->overlapped, sizeof(sendOver->overlapped));
    sendOver->operation = IOOperation::SEND;
    sendOver->wsaBuf.buf = reinterpret_cast<char*>(pkt);
    sendOver->wsaBuf.len = sizeof(ClearCountPacket);
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
float CalcDistance3D(const XMFLOAT3& A, const XMFLOAT3& B) {
    XMVECTOR VecA = XMLoadFloat3(&A);
    XMVECTOR VecB = XMLoadFloat3(&B);
    XMVECTOR Diff = XMVectorSubtract(VecA, VecB);
    XMVECTOR Length = XMVector3Length(Diff);
    return XMVectorGetX(Length);
}
void IOCompletionPort::WorkThread() {
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    OVERLAPPED* overlapped;
    int p = 0;
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
            
            std::cout << "입장idcount:" << idCount << std::endl;
            std::cout << "clientCount:" << clientCount << std::endl;
            NotifyOthersAboutNewClient(newClient);
            SendExistingClientsToNewClient(newClient);
            {
                std::lock_guard<std::mutex> lock(waitMutex);
                waitingClients.push_back(newClient);
                if (waitingClients.size() >= 3) {
                    std::vector<stClientInfo*> roomMembers(waitingClients.begin(), waitingClients.begin() + 3);
                    waitingClients.erase(waitingClients.begin(), waitingClients.begin() + 3);
                    CreateRoom(roomMembers);
                    for (auto& client : roomMembers) {
                        SendData_EnterRoom(client);  // 여기서 먼저 룸 ID를 클라이언트에 전송
                        //NotifyOthersAboutNewClient(client);
                        //SendExistingClientsToNewClient(client);
                        RegisterRecv(client);
                    }
                }
                else {
                    //SendData_EnterRoom(newClient);  // 대기 중인 상태 전송 (roomID == 0)
                    //NotifyOthersAboutNewClient(newClient );
                    //SendExistingClientsToNewClient(newClient);
                    //RegisterRecv(newClient);
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
                << client->id << " 접속 해제!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";

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
            auto it = rooms.find(client->roomID);
            if (it == rooms.end()) return;
            Room& room = it->second;
            if (*packetType == PacketType::MOVE) {
                MovePacket_CtoS* movePacket = reinterpret_cast<MovePacket_CtoS*>(pOverlappedEx->buffer);

                client->x = movePacket->x;
                client->y = movePacket->y;
                client->z = movePacket->z;

               //auto it = rooms.find(client->roomID);
               //if (it == rooms.end()) break;  // 해당 룸이 없다면 무시
               //Room& room = it->second;

                XMFLOAT3 arrivePosition{ 120.0f, 0.0f, 94.0f };

                {
                    std::lock_guard<std::mutex> lock(*room.roomMutex);
                    int arrivedCount = 0;

                    for (auto* c : room.clients) {
                        if (!c) continue;

                        XMFLOAT3 playerPosition{ c->x, c->y, c->z };
                        float distance = CalcDistance3D(playerPosition, arrivePosition);

                        bool prev = c->curr;
                        c->curr = (distance <= 40.0f);

                        if (c->curr != prev) {
                            SendData_PlayerArrivalPacket(c, c->id, c->curr);
                            c->prev = c->curr;
                        }

                        if (c->curr)
                            arrivedCount++;
                    }

                    // clearCount 갱신
                    room.clearCount = arrivedCount;

                    // 모든 room 클라이언트에게 clearCount 전송
                    for (auto* c : room.clients) {
                        if (c) SendData_ClearCountPacket(c, room.clearCount);
                    }
                }

                // 이동 패킷을 같은 방 클라이언트들에게만 전송
                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_Move(client, otherClient);
                }
            }
            else if (*packetType == PacketType::VIEW_ANGLE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                //ViewingAnglePacket_CtoS* viewAnglePacket = reinterpret_cast<ViewingAnglePacket_CtoS*>(pOverlappedEx->buffer);
                //
                //client->angle_x = viewAnglePacket->x;
                //client->angle_y = viewAnglePacket->y;
                //client->angle_z = viewAnglePacket->z;
                ////std::cout << "[시선] 클라이언트 " << client->id
                ////    << " 시선각도: (" << client->angle_x << ", " << client->angle_y << ", " << client->angle_z << ")\n";
                //
                //// 이동 패킷을 모든 클라이언트에게 전송
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_ViewAngle(client, otherClient);
                //    }
                //}
                ViewingAnglePacket_CtoS* pkt = reinterpret_cast<ViewingAnglePacket_CtoS*>(pOverlappedEx->buffer);
                client->angle_x = pkt->x;
                client->angle_y = pkt->y;
                client->angle_z = pkt->z;

                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_ViewAngle(client, otherClient);
                }

            }
            else if (*packetType == PacketType::ANIMATION) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                //AnimationPacket_CtoS* anyPacket = reinterpret_cast<AnimationPacket_CtoS*>(pOverlappedEx->buffer);
                //
                //client->animationType = anyPacket->anymationType;
                //
                ////std::cout << "[애니매이션] 클라이언트 " << client->animationType << " \n";
                //
                //// 이동 패킷을 모든 클라이언트에게 전송
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_Animaion(client, otherClient);
                //    }
                //}
                AnimationPacket_CtoS* pkt = reinterpret_cast<AnimationPacket_CtoS*>(pOverlappedEx->buffer);
                client->animationType = pkt->anymationType;

                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_Animaion(client, otherClient);
                }
            }
            else if (*packetType == PacketType::MONSTER_STATE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                //MonsterStatePacket_CtoS* pkt = reinterpret_cast<MonsterStatePacket_CtoS*>(pOverlappedEx->buffer);
                
                //if (defenseState) {
                //
                //    {
                //        std::lock_guard<std::mutex> lock(defenseMonsterMutex);
                //        defenseMonsters[pkt->id].state = pkt->state;
                //    }
                //
                //}
                //else {
                //    myMonsters[pkt->id].state = pkt->state;
                //}
                
                MonsterStatePacket_CtoS* pkt = reinterpret_cast<MonsterStatePacket_CtoS*>(pOverlappedEx->buffer);
                std::cout << pkt->id << " --  Monstertype:" << pkt->Mtype << " monID:" << pkt->id << ", state:" << pkt->state << std::endl;
                std::lock_guard<std::mutex> lock(*room.roomMutex);
                if (room.defenseState)
                    room.defenseMonsters[pkt->id].state = pkt->state;
                else
                    room.myMonsters[pkt->id].state = pkt->state;

                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_MonsterState(otherClient, pkt->Mtype, pkt->state, pkt->id);
                }
            }
            else if (*packetType == PacketType::MONSTER_MOVE) {
                //if (bytesTransferred < sizeof(MovePacket)) {
                //    std::cerr << "[에러] MOVE 패킷 크기 오류: " << bytesTransferred << " bytes" << std::endl;
                //    continue;
                //}

                //MonsterMovePacket* pkt = reinterpret_cast<MonsterMovePacket*>(pOverlappedEx->buffer);
                 //std::cout <<"MonsterID:" << pkt->monsterId<<"pID"<<pkt->playerId<<  "(" << pkt->x << ", " << pkt->x << ", " << pkt->y << ", "
                 //    << pkt->z <<" angle:"<< pkt->angle_y<< std::endl;
                
                // 데미지 패킷을 모든 클라이언트에게 전송
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_MonsterMove(otherClient, pkt->x, pkt->y, pkt->z, pkt->angle_y,pkt->monsterId, pkt->playerId);
                //    }
                //}

                MonsterMovePacket* pkt = reinterpret_cast<MonsterMovePacket*>(pOverlappedEx->buffer);
                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_MonsterMove(otherClient, pkt->x, pkt->y, pkt->z, pkt->angle_y, pkt->monsterId, pkt->playerId);
                }
            }
            else if (*packetType == PacketType::PTOM_DAMAGE) {
                
                //std::cout << "[DEBUG] 받은 바이트 수: " << bytesTransferred << std::endl;
                PtoMDamagePacket* pkt = reinterpret_cast<PtoMDamagePacket*>(pOverlappedEx->buffer);
                int sendHP = 0;
                {
                    std::lock_guard<std::mutex> lock(*room.roomMutex);
                    if (room.defenseState) {
                        //if (0 <= pkt->monsterID && pkt->monsterID < DEFENSE_MONSTER) {
                        auto& m = room.defenseMonsters[pkt->monsterID];
                        m.hp -= pkt->attackHp;
                        if (m.hp < 0) m.hp = 0;
                        sendHP = m.hp;
                        if (std::all_of(room.defenseMonsters.begin(), room.defenseMonsters.end(), [](const MonsterData& m) { return m.hp <= 0; }))
                            room.defenseState = false;
                        // }
                    }
                    else {
                        //if (pkt->monsterID<=1000) {
                        auto& m = room.myMonsters[pkt->monsterID];
                        m.hp -= pkt->attackHp;
                        if (m.hp < 0) m.hp = 0;
                        sendHP = m.hp;
                        //}
                    }
                }
                for (auto* otherClient : room.clients) {
                    if (!otherClient) continue;
                    SendData_PtoMDamagePacket(otherClient, pkt->monsterID, sendHP);
                }
                //int sendHP;
                //if (defenseState) {
                //    //if (0 <= pkt->monsterID && pkt->monsterID <= 19) {
                //        {
                //            std::lock_guard<std::mutex> lock(defenseMonsterMutex);
                //            defenseMonsters[pkt->monsterID].hp -= pkt->attackHp;
                //            if (defenseMonsters[pkt->monsterID].hp < 0)
                //                defenseMonsters[pkt->monsterID].hp = 0;
                //        }
                //        std::cout << "MonstersHP:" << defenseMonsters[pkt->monsterID].hp << std::endl;
                //
                //        bool allDead = true;
                //        for (const MonsterData& m : defenseMonsters) {
                //            if (m.hp > 0) { // 하나라도 살아있으면 allDead를 false로
                //                allDead = false;
                //                break;
                //            }
                //        }
                //
                //        if (allDead) {
                //            defenseState = false;
                //            std::cout << "[알림] 모든 방어 몬스터가 사망하여 defenseState가 false로 전환되었습니다." << std::endl;
                //        }
                //        sendHP = defenseMonsters[pkt->monsterID].hp;
                //        for (stClientInfo* otherClient : clients) {
                //            if (!otherClient) continue;
                //            if (true /*otherClient != client*/ /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //                SendData_PtoMDamagePacket(otherClient, pkt->monsterID, sendHP);
                //            }
                //        }
                //   // }
                //}
                //else {
                //    //if (pkt->monsterID<=1000) {
                //        myMonsters[pkt->monsterID].hp -= pkt->attackHp;
                //        std::cout << "MonstersHP:" << myMonsters[pkt->monsterID].hp << std::endl;
                //        if (myMonsters[pkt->monsterID].hp < 0)
                //            myMonsters[pkt->monsterID].hp = 0;
                //
                //        sendHP = myMonsters[pkt->monsterID].hp;
                //        for (stClientInfo* otherClient : clients) {
                //            if (!otherClient) continue;
                //            if (true /*otherClient != client*/ /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //                SendData_PtoMDamagePacket(otherClient, pkt->monsterID, sendHP);
                //            }
                //        }
                //    //}
                //}
                //// 데미지 패킷을 모든 클라이언트에게 전송
                
               
            }

            else if (*packetType == PacketType::MTOP_DAMAGE) {

                //MtoPDamagePacket* pkt = reinterpret_cast<MtoPDamagePacket*>(pOverlappedEx->buffer);
                //
                //// 데미지 패킷을 모든 클라이언트에게 전송
                //int myHP;
                //for (stClientInfo* targetClient : clients) {
                //    if (!targetClient) continue;
                //    if (targetClient->id == pkt->playerID /* && targetClient->roomID == client->roomID */) {
                //
                //        // 서버에서 체력 계산
                //        targetClient->hp -= pkt->attackHp;
                //        if (targetClient->hp < 0)
                //            targetClient->hp = 0;
                //        myHP = targetClient->hp;
                //
                //        break;
                //    }
                //}
                //
                //std::cout << pkt->playerID <<"---실제체력:" << myHP << std::endl;
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (true /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_MtoPDamagePacket(otherClient, pkt->playerID, pkt->monsterID, myHP);
                //    }
                //}
                MtoPDamagePacket* pkt = reinterpret_cast<MtoPDamagePacket*>(pOverlappedEx->buffer);
                int myHP = 0;
                {
                    std::lock_guard<std::mutex> lock(*room.roomMutex);
                    for (auto* targetClient : room.clients) {
                        if (!targetClient) continue;
                        if (targetClient->id == pkt->playerID) {
                            targetClient->hp -= pkt->attackHp;
                            if (targetClient->hp < 0) targetClient->hp = 0;
                            myHP = targetClient->hp;
                            break;
                        }
                    }
                }
                for (auto* otherClient : room.clients) {
                    if (!otherClient) continue;
                    SendData_MtoPDamagePacket(otherClient, pkt->playerID, pkt->monsterID, myHP);
                }
               
            }
            else if (*packetType == PacketType::ENGINEER_INSTALL) {

                //EngineerInstallPacket* pkt = reinterpret_cast<EngineerInstallPacket*>(pOverlappedEx->buffer);
                //
                //// 데미지 패킷을 모든 클라이언트에게 전송
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_EngineerInstallPacket(otherClient, pkt->Etype, pkt->ID, pkt->rotY, pkt->posX, pkt->posY, pkt->posZ);
                //    }
                //}
                EngineerInstallPacket* pkt = reinterpret_cast<EngineerInstallPacket*>(pOverlappedEx->buffer);
                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_EngineerInstallPacket(otherClient, pkt->Etype, pkt->ID, pkt->rotY, pkt->posX, pkt->posY, pkt->posZ);
                }
            }
            else if (*packetType == PacketType::ENGINEER_OBJECT) {

                //EngineerObjectPacket* pkt = reinterpret_cast<EngineerObjectPacket*>(pOverlappedEx->buffer);
                //
                //// 데미지 패킷을 모든 클라이언트에게 전송
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_EngineerObjectPacket(otherClient, pkt->ID, pkt->hp);
                //    }
                //}
                EngineerObjectPacket* pkt = reinterpret_cast<EngineerObjectPacket*>(pOverlappedEx->buffer);
                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_EngineerObjectPacket(otherClient, pkt->ID, pkt->hp);
                }
            }
            else if (*packetType == PacketType::CENTER_HP) {

                //CenterBuildingPacket* pkt = reinterpret_cast<CenterBuildingPacket*>(pOverlappedEx->buffer);
                //
                //
                //auto it = rooms.find(client->roomID);
                //if (it == rooms.end()) break;
                //Room& room = it->second;
                //{
                //    std::lock_guard<std::mutex> lock(*room.roomMutex);
                //    room.centerHp -= pkt->damage;
                //    if (room.centerHp < 0) room.centerHp = 0;
                //}
                ////roomHp -= pkt->damage;
                ////std::cout << "room_hp:" << roomHp << std::endl;
                //// 데미지 패킷을 모든 클라이언트에게 전송
                //for (stClientInfo* otherClient : clients) {
                //    if (!otherClient) continue;
                //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
                //        SendData_CenterBuildingPacket(otherClient, roomHp);
                //    }
                //}
                CenterBuildingPacket* pkt = reinterpret_cast<CenterBuildingPacket*>(pOverlappedEx->buffer);
                {
                    std::lock_guard<std::mutex> lock(*room.roomMutex);
                    room.centerHp -= pkt->damage;
                    if (room.centerHp < 0) room.centerHp = 0;
                }
                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_CenterBuildingPacket(otherClient, room.centerHp);
                }
            }
            else if (*packetType == PacketType::GRENADE) {

               //GrenadePacket* pkt = reinterpret_cast<GrenadePacket*>(pOverlappedEx->buffer);
               //
               //// 데미지 패킷을 모든 클라이언트에게 전송
               //for (stClientInfo* otherClient : clients) {
               //    if (!otherClient) continue;
               //    if (otherClient != client /*&& client->roomID == otherClient->roomID*/) { // 패킷을 보낸 클라이언트에게는 다시 전송하지 않음
               //        SendData_GrenadePacket(otherClient, pkt->posX, pkt->posY, pkt->posZ, pkt->rotX, pkt->rotY, pkt->rotZ);
               //    }
               //}
               GrenadePacket* pkt = reinterpret_cast<GrenadePacket*>(pOverlappedEx->buffer);
               for (auto* otherClient : room.clients) {
                   if (!otherClient || otherClient == client) continue;
                   SendData_GrenadePacket(otherClient, pkt->posX, pkt->posY, pkt->posZ, pkt->rotX, pkt->rotY, pkt->rotZ);
               }
            }
            else if (*packetType == PacketType::CHOOSE_JOB) {

                ChooseJobPacket* pkt = reinterpret_cast<ChooseJobPacket*>(pOverlappedEx->buffer);

                client->job = pkt->job;
                
                for (auto* otherClient : room.clients) {
                    if (!otherClient || otherClient == client) continue;
                    SendData_ChooseJobPacket(otherClient, pkt->playerID, pkt->job);
                }
            }
           

            RegisterRecv(client);  // 다시 수신 대기
            if (pOverlappedEx->cleanup) {
                pOverlappedEx->cleanup();
            }
            else {
                delete pOverlappedEx;
            }
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
    if (randomPositionThread.joinable()) randomPositionThread.join();

    std::cout << "서버 종료 완료.\n";
}
