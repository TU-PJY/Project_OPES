#pragma once
#define NOMINMAX
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include<array>
#include <mutex>
#include <thread>
#include <unordered_map>
#include<algorithm>
#include <chrono>
#include<functional>
#include <memory>
#include "TerrainUtil.h"
#include "ScriptUtil.h"
#include"protocol.h"
#include"Packet.h"
#include "Timer.h"
#define MAX_SOCKBUF 1024  
#define SERVER_PORT 9000
//#define MAX_WORKERTHREAD 4  
constexpr std::size_t MAX_CLIENTS = 5000;

class TimerService;

// 몬스터의 타입과 초기 스폰 위치를 저장하는 구조체
struct MonsterData {
    int monsterType;
    float createPointX;
    float createPointZ;
    unsigned int id = 0;
    int targetClientId = -1;
    unsigned int state = 0;
    int hp=100;
    MonsterData() {
        //hp = 100;
        state = 0;

    };
};

enum class IOOperation {
    ACCEPT,  // 추가됨
    RECV,
    SEND
};

struct stOverlappedEx {
    WSAOVERLAPPED overlapped;
    WSABUF wsaBuf;
    char buffer[MAX_SOCKBUF];
    IOOperation operation;
    SOCKET acceptSocket; // AcceptEx에서 필요
    std::function<void()> cleanup;
};

struct stClientInfo {
    SOCKET socketClient;
    int id;
    float x, y,z;  
    float angle_x, angle_y, angle_z;
    unsigned short animationType;
    int stageState;
    bool ready;
    int job;
    bool prev;
    bool curr;
    int hp;
    int roomID;
    int name_size;
    char name[20];
    std::atomic<bool> alreadyRemoved{ false };

    int prevRemainSize = 0;
    char prevBuffer[MAX_SOCKBUF] = { 0 };
    stClientInfo() {
        socketClient = INVALID_SOCKET;
        roomID = -1;
        x = -130.0;
        y = 20;
        z = -130.0;
        angle_x=0, angle_y=0, angle_z=0;
        animationType = 0;
        stageState = 1;
        hp = 0;
        prev=false;
        curr=false;
        ready = false;
        //-130.0, 20.0, -130.0 
    }
};


struct Room {
    int roomID=-1;
    std::vector<stClientInfo*> clients;
    std::vector<MonsterData> myMonsters;
    std::vector<MonsterData> defenseMonsters{ DEFENSE_MONSTER1};

    std::vector<MonsterData> myMonsters2;
    std::vector<MonsterData> defenseMonsters2{ DEFENSE_MONSTER2 };

    std::vector<MonsterData> myMonsters3;
    std::vector<MonsterData> defenseMonsters3{ DEFENSE_MONSTER3 };

    int centerHp = CENTER_HP;
    int clearCount = 0;
    bool defenseState = false;
    //bool isCreat = true;
    int stageState = 1;
    int DeathCount = 0;


    std::shared_ptr<std::mutex> roomMutex = std::make_shared<std::mutex>();


    int nextMonsterToSend = 0;
    bool monsterRandomSent = false;
    std::chrono::steady_clock::time_point defenseStartTime = std::chrono::steady_clock::time_point::min();
    //std::cout << defenseStartTime << "!!!\n";
    std::chrono::steady_clock::time_point lastMonsterSendTime = std::chrono::steady_clock::time_point::min();

    //
    bool timerRunning = false;
    int  timerRemainSec = 0;
    std::chrono::steady_clock::time_point timerLastTick{};
    //
    int fileCount = 0;
};

struct DelayedRemoveEntry {
    stClientInfo* client;
    std::chrono::steady_clock::time_point timeScheduled;
};



class IOCompletionPort {
public:
    IOCompletionPort();
    ~IOCompletionPort();
    bool InitSocket();
    bool BindandListen();
    bool StartServer();
    void DestroyThread();
    void RegisterRecv(stClientInfo* client);
    void SendData(stClientInfo* sendingClient, stClientInfo* recvingClient, const char* message, int length);
    
    
    void SendData_Move(stClientInfo* sendingClient, stClientInfo* recvingClient);
    void SendData_ViewAngle(stClientInfo* sendingClient, stClientInfo* recvingClient);
    void SendData_Animaion(stClientInfo* sendingClient, stClientInfo* recvingClient);
    void SendData_EnterRoom(stClientInfo* recvingClient);
    void NotifyOthersAboutNewClient(stClientInfo* newClient);
    void SendExistingClientsToNewClient(stClientInfo* newClient);
    void SendData_MonsterState(stClientInfo* recvingClient,unsigned int monsterType, unsigned int monsterState, unsigned int id);
    void SendData_MonsterMove(stClientInfo* receiver, float x, float y, float z, float angle, unsigned int monsterId, unsigned int playerId);
    void SendData_MonsterMoveToAllClients(const MonsterData& m);

    void SendData_PtoMDamagePacket(stClientInfo* receiver, unsigned int monsterID, int attackHp);
    void SendData_MtoPDamagePacket(stClientInfo* receiver, unsigned int playerID, unsigned int monsterID, int attackHp);
    void SendData_EngineerInstallPacket(stClientInfo* receiver, int type, unsigned int ID, float rotY, float posX, float posY, float posZ);
    void SendData_EngineerObjectPacket(stClientInfo* receiver, unsigned int ID, int hp);
    void SendData_GrenadePacket(stClientInfo* receiver, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    void SendData_CenterBuildingPacket(stClientInfo* receiver, int hp);
    void SendData_PlayerArrivalPacket(stClientInfo* receiver, unsigned int playerID, bool arrive);
    void SendData_ClearCountPacket(stClientInfo* receiver, int PlayerCount);
    void SendData_ChooseJobPacket(stClientInfo* receiver, unsigned int playerID, int job);
    void SendData_ReadyPacket(stClientInfo* receiver, unsigned int id);
    void SendData_BangPacket(stClientInfo* receiver, unsigned int id);
    void SendData_PlayerDeathPacket(stClientInfo* receiver, unsigned int id);
    void SendData_DisconnectPacket(stClientInfo* receiver, unsigned int id);
    void SendData_MasterKeytPacket(stClientInfo* receiver, int key);
    void SendData_AttackObjectPacket(stClientInfo* receiver, int id,int damage);
    void SendData_PlayerUpgradePacket(stClientInfo* receiver, int player_id, int random_id);
    //
    void CreateRoom(const std::vector<stClientInfo*>& members);


    int GetPacketSizeByType(PacketType type);
    void ProcessPacket(char* buffer, stClientInfo* client);

    bool AddClient(stClientInfo* c); 
    void RemoveClient(stClientInfo* c);

    void PostRemove(stClientInfo* client);
    void ProcessDelayedRemoves();

    //
    std::unordered_map<int, Room>& Rooms();
    std::mutex& RoomMapMutex();
    std::atomic_bool& RunningFlag();
    void BroadcastStageTimer(unsigned roomID, int stage, int remainingSeconds, bool running);

private:
    
    
    int idCount = 0;
    //std::vector<stClientInfo*> clients;
    std::array<stClientInfo*, MAX_CLIENTS> clients{};   
    std::size_t clientCount = 0;
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;
    //std::vector<std::thread> workerThreads;
    std::thread workerThread;
    std::thread randomPositionThread;
    std::thread randomPositionThread2;
    std::thread randomPositionThread3;

    //bool isRunning = true;
    std::atomic_bool isRunning{ true };
    std::mutex clientMutex;
    //
    std::vector<stClientInfo*> waitingClients;
    std::unordered_map<int, Room> rooms;
    int nextRoomID = 0;
    std::mutex waitMutex;
    std::mutex roomMutex;

    std::mutex removeQueueMutex;
    std::vector<DelayedRemoveEntry> removeQueue;

    std::unique_ptr<TimerService> timerService;

    void WorkThread();
    void PostAccept();
    void RandomPositionThread();
    void RandomPositionThread2();
    void RandomPositionThread3();

};
