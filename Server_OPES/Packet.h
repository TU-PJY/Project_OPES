#define MAX_SOCKBUF 1024
enum class PacketType {
    CHAT,
    MOVE
};

// ä�� ��Ŷ ����ü
struct ChatPacket_S {
    PacketType type;  // �׻� PacketType::CHAT
    unsigned int id;
    char message[MAX_SOCKBUF];
   
};
struct ChatPacket_R {
    PacketType type;  // �׻� PacketType::CHAT
    char message[MAX_SOCKBUF];

};
// �̵� ��Ŷ ����ü
struct MovePacket_R {
    PacketType type;  // �׻� PacketType::MOVE
    int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
};
struct MovePacket_S {
    PacketType type;  // �׻� PacketType::MOVE
    unsigned int id;
    int x;
    int y;
};
