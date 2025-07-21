#pragma once
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
#include "TerrainUtil.h"
#include "ScriptUtil.h"

#include"Packet.h"
#define MAX_SOCKBUF 1024  
#define SERVER_PORT 9000
//#define MAX_WORKERTHREAD 4  
constexpr std::size_t MAX_CLIENTS = 5000;

// 몬스터의 타입과 초기 스폰 위치를 저장하는 구조체
struct MonsterData {
    int monsterType;
    float createPointX;
    float createPointZ;
    unsigned int id = 0;
    int targetClientId = -1;
    unsigned int state = 0;
    int hp;
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
    stOverlappedEx recvOverlapped;
    stOverlappedEx sendOverlapped;
    int roomID;
    std::atomic<bool> alreadyRemoved{ false };
    stClientInfo() {
        ZeroMemory(&recvOverlapped, sizeof(stOverlappedEx));
        ZeroMemory(&sendOverlapped, sizeof(stOverlappedEx));
        socketClient = INVALID_SOCKET;
        roomID = 0;
        x = -130.0;
        y = 20;
        z = -130.0;
        angle_x=0, angle_y=0, angle_z=0;
        animationType = 0;
        //-130.0, 20.0, -130.0 
    }
};


struct Room {
    int roomID;
    std::vector<stClientInfo*> clients;
    std::vector<MonsterData> monsters;
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
    void SendData_Player2Monster(unsigned int monsterID, unsigned int damage, stClientInfo* recvingClient);
    void SendData_EnterRoom(stClientInfo* recvingClient);
    void NotifyOthersAboutNewClient(stClientInfo* newClient);
    void SendExistingClientsToNewClient(stClientInfo* newClient);
    void SendData_MonsterState(stClientInfo* recvingClient,unsigned int monsterType, unsigned int monsterState, unsigned int id);
    void SendData_MonsterMove(stClientInfo* receiver, float x, float y, float z, float angle, unsigned int monsterId, unsigned int playerId);
    void SendData_MonsterMoveToAllClients(const MonsterData& m);

    void SendData_MonsterHp(stClientInfo* receiver, int currentHP, unsigned int monsterID);
    void SendData_PlantAnimationTimePacket(stClientInfo* receiver, float time);
    void SendData_PlayerHp(stClientInfo* receiver, int currentHP, unsigned int platerID);
    void SendData_EngineerInstallPacket(stClientInfo* receiver, int type, unsigned int ID, float rotY, float posX, float posY, float posZ);
    void SendData_EngineerObjectPacket(stClientInfo* receiver, unsigned int ID, int hp);
    void SendData_GrenadePacket(stClientInfo* receiver, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    void SendData_CenterBuildingPacket(stClientInfo* receiver, int hp);
    void SendData_PlayerArrivalPacket(stClientInfo* receiver, unsigned int playerID);
    //
    void CreateRoom(const std::vector<stClientInfo*>& members);


    bool AddClient(stClientInfo* c); 
    void RemoveClient(stClientInfo* c);

    void NPCAIThread();
private:
    
    
    int idCount = 0;
    //std::vector<stClientInfo*> clients;
    std::array<stClientInfo*, MAX_CLIENTS> clients{};   
    std::size_t clientCount = 0;
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;
    //std::vector<std::thread> workerThreads;
    std::thread accepterThread;
    std::thread workerThread;
    bool isRunning = true;
    std::mutex clientMutex;
    //
    std::vector<stClientInfo*> waitingClients;
    std::unordered_map<int, Room> rooms;
    int nextRoomID = 0;
    std::mutex waitMutex;
    std::mutex roomMutex;

    void WorkThread();
    void PostAccept();
    
    //void AcceptThread();

};
