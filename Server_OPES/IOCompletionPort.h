#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include <mutex>
#include <thread>
#include <unordered_map>
#define MAX_SOCKBUF 1024  
#define SERVER_PORT 9000
//#define MAX_WORKERTHREAD 4  

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

struct stClientInfo {
    SOCKET socketClient;
    int id;
    float x, y,z;  
    float angle_x, angle_y, angle_z;
    stOverlappedEx recvOverlapped;
    stOverlappedEx sendOverlapped;
    int roomID;

    stClientInfo() {
        ZeroMemory(&recvOverlapped, sizeof(stOverlappedEx));
        ZeroMemory(&sendOverlapped, sizeof(stOverlappedEx));
        socketClient = INVALID_SOCKET;
        roomID = 0;
        x = -130.0;
        y = 20;
        z = -130.0;
        angle_x=0, angle_y=0, angle_z=0;
        //-130.0, 20.0, -130.0 
    }
};

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
    void SendData_EnterRoom(stClientInfo* recvingClient);
    void RemoveClient(stClientInfo* client);
    //
    void CreateRoom(const std::vector<stClientInfo*>& members);
private:
    
    
    int idCount = 0;
    std::vector<stClientInfo*> clients;
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
    void AcceptThread();

};
