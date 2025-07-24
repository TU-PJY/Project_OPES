// 기지 체력
constexpr int                  CENTER_HP		= 500;

// 무기
// DAMAGE 데미지, RPM 연사속도(분당 발사수), RELOAD 재장전 속도, MAGAZINE 장탄수, HIP 견착, ADS 정조준

// 기관총
constexpr unsigned int         LMG_DAMAGE		= 15;
constexpr float                LMG_RPM			= 600.0f;
constexpr float                LMG_RELOAD		= 5.0f;
constexpr unsigned int         LMG_MAGAZINE		= 300;

// 지정 사수 소총
constexpr unsigned int         DMR_DAMAGE_HIP	= 40;
constexpr float                DMR_RPM_HIP		= 120.0f;
constexpr unsigned int         DMR_DAMAGE_ADS	= 300;
constexpr float                DMR_RPM_ADS		= 24.0f;
constexpr float                DMR_RELOAD		= 2.0f;
constexpr unsigned int         DMR_MAGAZINE		= 10;

// 샷건
constexpr unsigned int         SG_DAMAGE		= 10;
constexpr unsigned int         SG_FRAG			= 12;    // FRAG 파편, PIECE 조각, BUCK 산탄, PELLET 펠릿 용어 아직 못정함.
constexpr float                SG_RPM			= 60.0f;
constexpr float                SG_RELOAD		= 0.5f;
constexpr unsigned int         SG_MAGAZINE		= 6;

// 설치 무기
// DAMAGE 데미지, DURABILITY 내구도, INSTALL 설치 시간, RPM 연사속도, COOLDOWN 파괴된 후 쿨타임

// 포탑(TURRET)
constexpr unsigned int         TURRET_DAMAGE	= 15;
constexpr unsigned int         TURRET_DURABILITY= 150;
constexpr float                TURRET_INSTALL	= 5.0f;
constexpr float                TURRET_RPM		= 300.0f;
constexpr float                TURRET_COOLDOWN	= 10.0f;

// 스파이크(SPIKE)
constexpr unsigned int         SPIKE_DAMAGE		= 0;
constexpr unsigned int         SPIKE_DURABILITY = 20;
constexpr float                SPIKE_INSTALL	= 5.0f;
constexpr float                SPIKE_RPM		= 0.0f;
constexpr float                SPIKE_COOLDOWN	= 10.0f;

// 캐릭터 LMG 기관총 사수 / DMR 지정사수 / ENG 엔지니어
// HP 체력, SPEED 이동속도(km/h)
constexpr unsigned int         LMGUSER_HP		= 300;
constexpr float                LMGUSER_SPEED	= 8.0f;

constexpr unsigned int         DMRUSER_HP		= 125;
constexpr float                DMRUSER_SPEED	= 12.0f;

constexpr unsigned int         ENGUSER_HP		= 150;
constexpr float                ENGUSER_SPEED	= 10.0f;

// 몬스터
// HP 체력, SPEED 이동속도(km/h), DAMAGE 데미지, ATTACK 주기(초)

// 스테이지 1
// Plant Monster
constexpr unsigned int         PLANT_HP			= 100;
constexpr float                PLANT_SPEED		= 0.0f;     // 이동하지 않음
constexpr unsigned int         PLANT_DAMAGE		= 5;
constexpr float                PLANT_ATTACK		= 2.0f;     // 2초당 1회 공격

// Scorpion
constexpr unsigned int         SCORPION_HP		= 300;
constexpr float                SCORPION_SPEED	= 8.0f;
constexpr unsigned int         SCORPION_DAMAGE	= 20;
constexpr float                SCORPION_ATTACK	= 1.0f;

// 스테이지 2
// Troll
constexpr unsigned int         TROLL_HP			= 250;
constexpr float                TROLL_SPEED		= 10.0f;
constexpr unsigned int         TROLL_DAMAGE		= 25;
constexpr float                TROLL_ATTACK		= 1.0f;

// Treant
constexpr unsigned int         TREANT_HP		= 600;
constexpr float                TREANT_SPEED		= 6.0f;
constexpr unsigned int         TREANT_DAMAGE	= 75;
constexpr float                TREANT_ATTACK	= 2.0f;

// 스테이지 3
// Imp
constexpr unsigned int         IMP_HP			= 300;
constexpr float                IMP_SPEED		= 10.0f;
constexpr unsigned int         IMP_DAMAGE		= 15;
constexpr float                IMP_ATTACK		= 0.5f;

// Devil
constexpr unsigned int         DEVIL_HP			= 1000;
constexpr float                DEVIL_SPEED		= 7.0f;
constexpr unsigned int         DEVIL_DAMAGE		= 50;
constexpr float                DEVIL_ATTACK		= 2.0f;
