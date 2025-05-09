#define MAX_SOCKBUF 1024
enum class PacketType {
    CHAT,
    MOVE,
    VIEW_ANGLE,
    ENTER
};

// 채팅 패킷 구조체
struct ChatPacket_StoC {
    PacketType type;  // 항상 PacketType::CHAT
    unsigned int id;
    char message[MAX_SOCKBUF];

};
struct ChatPacket_CtoS {
    PacketType type;  // 항상 PacketType::CHAT
    char message[MAX_SOCKBUF];

};
// 이동 패킷 구조체
struct MovePacket_CtoS {
    PacketType type;  // 항상 PacketType::MOVE
    //int direction;    // 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT
    float x;
    float y;
    float z;
};
struct MovePacket_StoC {
    PacketType type;  // 항상 PacketType::MOVE
    unsigned int id;
    float x;
    float y;
    float z;
};
struct ViewingAnglePacket_CtoS {
    PacketType type;
    float x;
    float y;
    float z;
};
struct ViewingAnglePacket_StoC {
    PacketType type;  
    unsigned int id;
    float x;
    float y;
    float z;
};

struct EnterRoomPacket {
    PacketType type;
    unsigned int roomID;
};
