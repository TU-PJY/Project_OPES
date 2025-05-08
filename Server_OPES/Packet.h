#define MAX_SOCKBUF 1024
enum class PacketType {
    CHAT,
    MOVE,
    ENTER
};

// ä�� ��Ŷ ����ü
struct ChatPacket_StoC {
    PacketType type;  // �׻� PacketType::CHAT
    unsigned int id;
    char message[MAX_SOCKBUF];
   
};
struct ChatPacket_CtoS {
    PacketType type;  // �׻� PacketType::CHAT
    char message[MAX_SOCKBUF];

};
// �̵� ��Ŷ ����ü
struct MovePacket_CtoS {
    PacketType type;  // �׻� PacketType::MOVE
    //int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
    int x;
    int y;
};
struct MovePacket_StoC {
    PacketType type;  // �׻� PacketType::MOVE
    unsigned int id;
    int x;
    int y;
};
struct EnterRoomPacket {
    PacketType type;
    unsigned int roomID;
};
