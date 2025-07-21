#define MAX_SOCKBUF 1024
constexpr int MAX_NPC = 100;

enum class PacketType {
    CHAT,
    MOVE,
    VIEW_ANGLE,
    NEW_CLIENT,
    EXISTING_CLIENTS,
    ENTER,
    ANIMATION,
    PLAYER_TO_MOSTER,
    MONSTER_STATE,
    MONSTER_MOVE,
};

constexpr int MONSTER_TYPE1 = 1;//꽃
constexpr int MONSTER_TYPE2 = 2;//스콜피온

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
struct AnimationPacket_CtoS {
    PacketType type;
    unsigned short anymationType;
};
struct AnimationPacket_StoC {
    PacketType type;
    unsigned int id;
    unsigned short animationType;
};
struct EnterRoomPacket {
    PacketType type;
    unsigned int roomID;
    unsigned int myID;
};
struct NewClientPacket {
    PacketType type;
    unsigned int id;
};

struct ExistingClientsDataPacket {
    PacketType type;
    unsigned int count; // 클라이언트 수
    struct {
        unsigned int id;
        float x, y, z;
        float angle_x, angle_y, angle_z;
    } clients[1024]; // 또는 동적 크기로 관리
};

struct Player2Monster {
    PacketType type;
    unsigned int monsterId;
    unsigned int damage;
};

struct MonsterStatePacket_StoC {
    PacketType Ptype = PacketType::MONSTER_STATE;
    unsigned int state;
    unsigned int id;
};
struct MonsterStatePacket_CtoS {
    PacketType Ptype = PacketType::MONSTER_STATE;
    unsigned int Mtype;
    unsigned int state;
    unsigned int id;
};
struct MonsterMovePacket{
    PacketType type = PacketType::MONSTER_MOVE;
    unsigned int monsterId;
    unsigned int playerId;
};

