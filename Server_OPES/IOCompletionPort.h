#pragma once
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <vector>
#include <mutex>
#include <thread>

#define MAX_SOCKBUF 1024  
#define MAX_WORKERTHREAD 4  

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
    stOverlappedEx recvOverlapped;
    stOverlappedEx sendOverlapped;
    int roomID;

    stClientInfo() {
        ZeroMemory(&recvOverlapped, sizeof(stOverlappedEx));
        ZeroMemory(&sendOverlapped, sizeof(stOverlappedEx));
        socketClient = INVALID_SOCKET;
        roomID = -1;
    }
};

class IOCompletionPort {
public:
    IOCompletionPort();
    ~IOCompletionPort();
    bool InitSocket();
    bool BindandListen(int nBindPort);
    bool StartServer(const UINT32 maxClientCount);
    void DestroyThread();
    void RegisterRecv(stClientInfo* client);
    void SendData(stClientInfo* client, const char* message, int length);
private:
    std::vector<stClientInfo> clients;
    SOCKET listenSocket = INVALID_SOCKET;
    HANDLE iocpHandle = INVALID_HANDLE_VALUE;
    std::vector<std::thread> workerThreads;
    std::thread accepterThread;
    bool isRunning = true;

    void WorkerThread();
    void AcceptThread();
};
