#pragma once
#pragma pack(push, 1)

#define MAX_SOCKBUF 1024

enum class PacketType :unsigned int {
    CHAT,
    MOVE,
    VIEW_ANGLE,
    NEW_CLIENT,
    EXISTING_CLIENTS,
    ENTER,
    ANIMATION,
    MONSTER_STATE,
    MONSTER_MOVE,
    MTOP_DAMAGE,
    PTOM_DAMAGE,
    ENGINEER_INSTALL,
    ENGINEER_OBJECT,
    CENTER_HP,
    GRENADE,
    PLAYER_ARRIVAL,
    RANDOM_POSITION,
    CLEAR_COUNT,
    CHOOSE_JOB,
    READY,
    BANG,
    PLAYER_DEATH,
    DISCONNECT,
    FILE_LOAD,
    MASTER_KEY,
    NAME,
    ATTACK_OBJECT,
    UPGRADE,
    STAGE_TIMER,
    
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
struct NamePacket {
    PacketType type;
    int size;
    char name[20];
};
struct NewClientPacket {
    PacketType type;
    unsigned int id;
    int size;
    char name[20];
};

struct ExistingClientsDataPacket {
    PacketType type;
    unsigned int id;
    int size;
    char name[20];
   //unsigned int count; // 클라이언트 수
   //struct {
   //    unsigned int id;
   //} clients[1024]; // 또는 동적 크기로 관리
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
    float x;
    float y;
    float z;
    float angle_y;
};
//----------------------------------7/22
 // 플레이어가 몬스터 피격 시 전송
struct  PtoMDamagePacket {
    PacketType type;
    unsigned int monsterID;
    int attackHp;
};

// 몬스터가 플레이어 피격 시 전송
// 모든 플레이어 공통
struct MtoPDamagePacket {
    PacketType type;
    unsigned int playerID;
    unsigned int monsterID;
    int attackHp;
};

// 엔지니어 구조물 설치 시 전송
struct EngineerInstallPacket {
    PacketType Ptype;
    int Etype; // 구조물 타입
    unsigned int ID; // 구조물 아이디
    float rotY; // 구조물 y 회전
    float posX; // 구조물 설치 위치
    float posY;
    float posZ;
};

// 엔지니어 설치 구조물 체력 
// 대미지를 받을 때 마다 전송
struct EngineerObjectPacket {
    PacketType type;
    unsigned int ID;
    int hp;
};

// 디펜스 모드 중앙 건물 체력
struct CenterBuildingPacket{
    PacketType type;
    int damage;//서버->클라 = 센터체력 / 클라->서버 = 받은 데미지 보내줌
};


// 수류탄 던질 시 전송
// 받은 위치 및 각도로 생성
struct GrenadePacket {
    PacketType type;
    float posX; // 수류탄 생성 위치
    float posY;
    float posZ;

    float rotX; // 수류탄 생성 각도
    float rotY;
    float rotZ;
};

// 플레이어 끝지점 도착 여부
// 플레이어 도착 시 카운트가 1 증가
// 3명 플레이어가 도착하면 카운트가 3이 되어 다음 맵으로 전환

struct PlayerArrivalPacket {
    PacketType type;
    unsigned int playerID;
    bool arrive;
};
//디팬스 모드 플렌트 패킷
struct DefenseRandomPacket {
    PacketType type;
    unsigned int monsterID;
    float x;
    float z;
};

struct ClearCountPacket {
    PacketType type;
    int PlayerCount;
};

struct ChooseJobPacket {
    PacketType type;
    unsigned int playerID;
    int job;
};

struct ReadyPacket {
    PacketType type;
    unsigned int playerID;
};
struct BangPacket {
    PacketType type;
    unsigned int playerID;
};
struct PlayerDeathPacket {
    PacketType type;
    unsigned int playerID;
};
struct DisconnectPacket {
    PacketType type;
    unsigned int playerID;
};
struct FilePacket {
    PacketType type;
    int stage;
};
struct MasterKeyPacket {
    PacketType type;
    int keyNum;
};
struct AttackObjectPacket {
    PacketType type;
    int id;
};
struct PlayerUpgradePacket {
    PacketType type;
    int player_id;
    int random_id;
};
struct StageTimerPacket {
    PacketType type = PacketType::STAGE_TIMER;
    unsigned int roomID;
    int stage;               // 현재 스테이지(1,2,3..)
    int remainingSeconds;    // 남은 시간(초, 0~60)
    bool running;            // true=진행중, false=0초 도달(최종 1회)
};
#pragma pack(pop)