// 기지 체력
constexpr int                  CENTER_HP		= 500;

// 무기
// DAMAGE 데미지, RPM 연사속도(분당 발사수), RELOAD 재장전 속도, MAGAZINE 장탄수, HIP 견착, ADS 정조준
// 총구 황염 렌더링 시간
constexpr float                FLAME_RENDER_TIME = 0.03f;

// 기관총
constexpr unsigned int         MG_DAMAGE	   = 10;
constexpr float                MG_RECOIL       = 2.5f;
constexpr float				   MG_RECOIL_BACK  = 0.1;
constexpr float                MG_SHOOT_DELAY  = 0.1f;
constexpr float                MG_RELOAD_TIME  = 3.0f;
constexpr unsigned int         MG_MAGAZINE	   = 100;

// 지정 사수 소총
constexpr unsigned int         DMR_DAMAGE_HIP	= 40;
constexpr float                DMR_RPM_HIP		= 120.0f;
constexpr unsigned int         DMR_DAMAGE_ADS	= 300;
constexpr float                DMR_RPM_ADS		= 24.0f;
constexpr float                DMR_RELOAD		= 2.0f;
constexpr unsigned int         DMR_MAGAZINE		= 10;

// 샷건
constexpr unsigned int         SG_DAMAGE		= 10;
constexpr float                SG_RECOIL        = 12.0f;
constexpr float                SG_RECOIL_BACK   = 0.2f;
constexpr unsigned int         SG_FRAG			= 12;    // FRAG 파편, PIECE 조각, BUCK 산탄, PELLET 펠릿 용어 아직 못정함.
constexpr float                SG_SHOOT_DELAY   = 1.0f;
constexpr float                SG_RELOAD_TIME	= 2.0f;
constexpr unsigned int         SG_MAGAZINE		= 6;

// 설치 무기
// DAMAGE 데미지, DURABILITY 내구도, INSTALL 설치 시간, RPM 연사속도, COOLDOWN 파괴된 후 쿨타임

// 포탑(TURRET)
constexpr unsigned int         TURRET_DAMAGE	= 10;
constexpr float                TURRET_DURABILITY= 20.0f;
constexpr float                TURRET_INSTALL	= 5.0f;
constexpr float                TURRET_RPM		= 0.2f;

// 스파이크(SPIKE)
//constexpr unsigned int         SPIKE_DAMAGE		= 0;
//constexpr unsigned int         SPIKE_DURABILITY = 20;
//constexpr float                SPIKE_INSTALL	= 5.0f;
//constexpr float                SPIKE_RPM		= 0.0f;
//constexpr float                SPIKE_COOLDOWN	= 10.0f;

// 캐릭터 LMG 기관총 사수 / DMR 지정사수 / ENG 엔지니어
// HP 체력, SPEED 이동속도(km/h)
constexpr unsigned int         CHARACTER_MG_HP		= 300;
constexpr float                CHARACTER_MG_SPEED   = 8.0f;

constexpr unsigned int         CHARACTER_DMR_HP		= 125;
constexpr float                CHARACTER_DMR_SPEED	= 12.0f;

constexpr unsigned int         CHARACTER_ENG_HP		= 150;
constexpr float                CHARACTER_ENG_SPEED	= 10.0f;

// 몬스터
// HP 체력, SPEED 이동속도(km/h), DAMAGE 데미지, ATTACK 주기(초)

// 스테이지 1
// Plant Monster
constexpr unsigned int         PLANT_HP			= 100;
constexpr float                PLANT_SPEED		= 0.0f;     // 이동하지 않음
constexpr unsigned int         PLANT_DAMAGE		= 5;
//constexpr float                PLANT_ATTACK		= 2.0f;     // 2초당 1회 공격

// Scorpion
constexpr unsigned int         SCORPION_HP		= 300;
constexpr float                SCORPION_SPEED	= 6.0f;
constexpr unsigned int         SCORPION_DAMAGE	= 20;
//constexpr float                SCORPION_ATTACK	= 1.0f;

// 스테이지 2
// Troll
constexpr unsigned int         TROLL_HP			= 250;
constexpr float                TROLL_SPEED		= 10.0f;
constexpr unsigned int         TROLL_DAMAGE		= 25;
//constexpr float                TROLL_ATTACK		= 1.0f;

// Treant
constexpr unsigned int         TREANT_HP		= 600;
constexpr float                TREANT_SPEED		= 6.0f;
constexpr unsigned int         TREANT_DAMAGE	= 75;
//constexpr float                TREANT_ATTACK	= 2.0f;

// 스테이지 3
// Imp
constexpr unsigned int         IMP_HP			= 300;
constexpr float                IMP_SPEED		= 10.0f;
constexpr unsigned int         IMP_DAMAGE		= 15;
constexpr float                IMP_ATTACK		= 0.5f;

// gazer
constexpr unsigned int         DEVIL_HP			= 200;
constexpr float                DEVIL_SPEED		= 7.0f;
constexpr unsigned int         DEVIL_DAMAGE		= 50;
constexpr float                DEVIL_ATTACK		= 2.0f;

// 보너스 & 페널티
// 공통 보너스
constexpr float BONUS_RPM = 1.2f;				// 무기 과열 제거 (Weapon Cooling), 무기 연사속도 +20%
constexpr float BONUS_RELOAD = 0.7f;			// 빠른 장전 훈련 (Quick Reload Training), 장전 속도 30% 감소
constexpr float BONUS_MOVE_SPEED = 1.15f;		// 아드레날린 러시 (Adrenaline Rush), 이동속도 +15%
constexpr int   BONUS_GRENADE = 2;				// 전술 확장 벨트 (Tactical Extended Pocket), 수류탄 +2

// 기관총병 보너스
constexpr float BONUS_RECOIL_LMG = 0.5f;		// 반동 제어 숙련 (Recoil Mastery), 반동 -50%
constexpr float BONUS_HP_LMG = 1.33f;			// 굳은 의지 (Strong Will), 체력 +33%

// 저격병 보너스
constexpr float BONUS_ADS_DAMAGE_DMR = 1.2f;    // 정조준 조준경 (ADS Scope Enhancement), 정조준 데미지 +20%
constexpr float BONUS_ADS_SPEED_DMR = 1.5f;		// 민첩한 조작 (Agile Handling), 정조준 전환 속도 +50%

// 엔지니어 보너스
constexpr float BONUS_STRUCTURE_HP_ENG = 1.5f;  // 적응형 구조 (Adaption Structure), 설치 구조물 체력 +50%
constexpr int   BONUS_PELLET_SG = 3;			// 집탄 모듈 확장 (Expanded Pellet Module), 산탄 수 +3


// 공통 페널티
constexpr float PENALTY_RPM = 0.85f;			// 무기 과열 (Weapon Overheating), 무기 연사속도 -15%
constexpr float PENALTY_RELOAD = 1.3f;			// 급한 장전 (Interrupted Reload), 장전 속도 +30%
constexpr float PENALTY_MOVE_SPEED = 0.85f;		// 피로 누적 (Fatigue Accumulation), 이동속도 -15%
constexpr int   PENALTY_GRENADE = -2;			// 탄띠 손상 (Damaged Utility Belt), 수류탄 -2

// 기관총병 페널티
constexpr float PENALTY_RECOIL_LMG = 1.25f;		// 연사 불안정 (Spray Instability), 반동 +25%

// 저격병 페널티
constexpr float PENALTY_RECOIL_DMR = 1.2f;		// 정조준 불안정 (ADS Instability), 반동 +20%
constexpr float PENALTY_ADS_RPM_DMR = 0.9f;		// 정조준 불안정 (ADS Instability), 정조준 연사속도 -10%
constexpr float PENALTY_HIP_RPM_DMR = 0.75f;	// 빠른 지향의 대가 (Hipfire Penalty), 지향사격 연사속도 -25%

// 엔지니어 페널티
constexpr float PENALTY_STRUCTURE_HP_ENG = 0.7f;// 불안정한 설계 (Flawed Structure), 설치 구조물 체력 -30%
constexpr float PENALTY_PELLET_DAMAGE_SG = 0.8f;// 불량 탄약 (Faulty Ammo), 샷건 펠릿당 데미지 -20%
