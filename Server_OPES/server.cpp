#include "IOCompletionPort.h"

const UINT16 SERVER_PORT = 3000;
//const UINT16 MAX_CLIENT = 100;

int main() {
    IOCompletionPort server;

    if (!server.InitSocket()) return -1;
    if (!server.BindandListen(SERVER_PORT)) return -1;
    if (!server.StartServer()) return -1;

    printf("서버 실행 중... 아무 키나 누르면 종료됩니다.\n");
    getchar();

    server.DestroyThread();
    return 0;
}
