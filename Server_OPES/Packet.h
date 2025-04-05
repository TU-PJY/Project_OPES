#define MAX_SOCKBUF 1024
enum class PacketType {
    CHAT,
    MOVE
};

// 채팅 패킷 구조체
struct ChatPacket_S {
    PacketType type;  // 항상 PacketType::CHAT
    unsigned int id;
    char message[MAX_SOCKBUF];
   
};
struct ChatPacket_R {
    PacketType type;  // 항상 PacketType::CHAT
    char message[MAX_SOCKBUF];

};
// 이동 패킷 구조체
struct MovePacket_R {
    PacketType type;  // 항상 PacketType::MOVE
    int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
};
struct MovePacket_S {
    PacketType type;  // 항상 PacketType::MOVE
    unsigned int id;
    int x;
    int y;
};
