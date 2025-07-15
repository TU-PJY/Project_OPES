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
#include"Packet.h"
#define MAX_SOCKBUF 1024  
#define SERVER_PORT 9000
//#define MAX_WORKERTHREAD 4  
constexpr std::size_t MAX_CLIENTS = 5000;

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
struct stNPC {
    unsigned int id;
    float x, y, z;
    float origin_x, origin_y, origin_z; //원래 위치 저장
    int hp;
    bool isAlive = true;
    float speed = 1.0f;
    int targetPlayerId = -1;

    std::mutex npcMutex;
   std::chrono::steady_clock::time_point lastSent = std::chrono::steady_clock::now();
};

extern std::vector<std::unique_ptr<stNPC>> npcs;
extern std::thread npcThread;
extern std::mutex npcMutex;

struct Room {
    int roomID;
    std::vector<stClientInfo*> clients;
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
    //
    void CreateRoom(const std::vector<stClientInfo*>& members);


    bool AddClient(stClientInfo* c); 
    void RemoveClient(stClientInfo* c);

    void NPCAIThread();
    void SendData_NPCState(const stNPC& npc);
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
