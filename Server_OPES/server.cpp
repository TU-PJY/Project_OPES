#include "IOCompletionPort.h"

const UINT16 SERVER_PORT = 3000;
//const UINT16 MAX_CLIENT = 100;

int main() {
    IOCompletionPort server;

    if (!server.InitSocket()) return -1;
    if (!server.BindandListen(SERVER_PORT)) return -1;
    if (!server.StartServer()) return -1;

    printf("���� ���� ��... �ƹ� Ű�� ������ ����˴ϴ�.\n");
    getchar();

    server.DestroyThread();
    return 0;
}
