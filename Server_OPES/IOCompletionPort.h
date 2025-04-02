#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include <mutex>
#include <thread>

#define MAX_SOCKBUF 1024  
//#define MAX_WORKERTHREAD 4  

enum class IOOperation {
    RECV,
    SEND
};
enum class PacketType {
    CHAT,
    MOVE
};

// 채팅 패킷 구조체
struct ChatPacket {
    PacketType type;  // 항상 PacketType::CHAT
    char message[MAX_SOCKBUF];
};

// 이동 패킷 구조체
struct MovePacket {
    PacketType type;  // 항상 PacketType::MOVE
    int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
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
    int x, y;  
    stOverlappedEx recvOverlapped;
    stOverlappedEx sendOverlapped;
    int roomID;

    stClientInfo() {
        ZeroMemory(&recvOverlapped, sizeof(stOverlappedEx));
        ZeroMemory(&sendOverlapped, sizeof(stOverlappedEx));
        socketClient = INVALID_SOCKET;
        roomID = -1;
        x = 0; 
        y = 0;
    }
};
class IOCompletionPort {
public:
    IOCompletionPort();
    ~IOCompletionPort();
    bool InitSocket();
    bool BindandListen(int nBindPort);
    bool StartServer();
    void DestroyThread();
    void RegisterRecv(stClientInfo* client);
    void SendData(stClientInfo* client, const char* message, int length);
    void RemoveClient(stClientInfo* client);
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

    void WorkThread();
    void AcceptThread();

};
