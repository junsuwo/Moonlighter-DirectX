#pragma once
#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <d3dx9.h>
#include "Hero.h"  // 히어로 포인터 사용을 위한 헤더 포함
#include <Windows.h>

constexpr int HAMMER_DAMAGE_AMOUNT = 20;  // 해머 공격 데미지 (원하는 값으로 조정)
constexpr int SLASH_DAMAGE_AMOUNT = 15;   // 슬래시 공격 데미지 (원하는 값으로 조정)

// ==============================
// Boss 전용 방향 enum
// (기존 Monster, Hero의 Direction과 충돌 방지)
// ==============================
enum class BossDirection {
    Up,
    Down,
    Left,
    Right
};

// ==============================
// 보스의 주요 상태 정의 (FSM)
// ==============================
enum class BossState {
    //Intro,  // 입장 연출용
    Idle,               // 대기 상태
    Chase,              // 플레이어 추적
    Attack,             // 공격 중 (Slash, Hammer 등 패턴 포함)
    Summon,             // 몬스터 소환
  
   
    Dead                // 사망
};

// ==============================
// 공격 패턴 정의
// ==============================
enum class BossAttackPattern {
    None,
    Slash,      // 검 휘두르기
    Hammer      // 망치 내려찍기
};

// ==============================
// BossSkeleton 클래스 선언
// ==============================
class BossSkeleton
{
protected:
    // 공격 처리 함수 (Update 내부에서 호출)
    void UpdateAttack(float deltaTime);

private:



    // ----------------------------
    // 애니메이션 관련 변수들
    // ----------------------------
    float m_moveSpeed = 300.f;     // 이동 속도
    float m_dirX = 0.f;            // 이동 방향 X
    float m_dirY = 1.f;            // 이동 방향 Y

    std::vector<LPDIRECT3DTEXTURE9> animFrames;  // 기본 프레임 모음
    float m_x{}, m_y{};  // 위치 좌표
    RECT m_heroRc;       // 히어로 충돌 박스
    std::vector<RECT> m_obstacleRects;
    // 현재 위치에서 충돌박스 계산 함수
    RECT CalcBossRectAt(float x, float y);

    // 이동 가능 여부 체크 함수
    bool CanMoveTo(float tryX, float tryY);


    static constexpr float BOSS_SCALE = 1.5f;

    // 이동 애니메이션 프레임 제어
    int m_moveFrame = 0;
    float m_moveFrameTimer = 0.f;
    float m_moveFrameTime = 0.02f;

    // 검 애니메이션
    int m_swordFrame = 0;
    float m_swordFrameTimer = 0.f;
    float m_swordFrameTime = 0.02f;

    // 해머 애니메이션
    int m_hammerFrame = 0;
    float m_hammerFrameTimer = 0.f;
    float m_hammerFrameTime = 0.02f;

    // 패턴 중복 방지용 사용 여부
    bool m_usedSlash = false;
    bool m_usedHammer = false;
    bool m_usedShockwave = false;
    bool m_usedJump = false;

    // 현재 선택된 공격 패턴
    BossAttackPattern m_currentAttackPattern = BossAttackPattern::None;

    // 방향별 애니메이션 프레임
    std::vector<LPDIRECT3DTEXTURE9> moveFramesUp, moveFramesDown, moveFramesLeft, moveFramesRight;
    std::vector<LPDIRECT3DTEXTURE9> swordFramesUp, swordFramesDown, swordFramesLeft, swordFramesRight;
    std::vector<LPDIRECT3DTEXTURE9> hammerFramesUp, hammerFramesDown, hammerFramesLeft, hammerFramesRight;
    
    // 패턴 사용 여부 초기화
    void ResetPatternUsage();
    bool AllPatternsUsed() const;

    // ----------------------------
    // FSM 상태 관리
    // ----------------------------
    BossState m_state = BossState::Idle;
    float m_stateTimer = 0.f;

    // ----------------------------
    // 점프 공격 관련
    // ----------------------------
    float m_jumpTimer = 0.f;
    float m_jumpDuration = 0.2f;
    float m_maxJumpHeight = 20.f;
    float m_originalPosY = 0.f;
    float m_jumpYOffset = 0.f;
    bool m_isJumping = false;

    // 슬래시 공격 정보
    const float SLASH_ATTACK_DURATION = 0.5f;
    const float WAIT_AFTER_ATTACK_DURATION = 0.2f;
    const float SLASH_DAMAGE_TIME = 0.5f;
    const int SLASH_DAMAGE_AMOUNT = 10;



    // ----------------------------
    // 충돌 박스 관련
    // ----------------------------
    RECT m_rc_boss;               // 전체 보스 박스
    RECT attackRangeRc_boss;     // 사정거리 범위
    RECT detectRangeRc_boss;     // 감지 범위
    RECT damageHitboxRc_boss;    // 공격 판정 범위
    RECT m_attackHitbox;
    // 충돌 계산 함수들
    void UpdateCollisionBoxes();
  

    // ----------------------------
    // 애니메이션 및 상태 플래그
    // ----------------------------
    BossDirection m_direction = BossDirection::Down;
    float attackAnimTimer = 0.f;
    int attackAnimFrame = 0;
    bool m_hasDealtDamageThisAttack = false;
    static constexpr float MOVE_FRAME_DURATION = 0.02f;
  
    // ----------------------------
    // 타겟용 히어로 포인터
    // ----------------------------
    Hero* m_hero = nullptr;

    float m_targetX = 0.f;
    float m_targetY = 0.f;
    int m_attackDamage = 10;
public:
    // 생성자 / 소멸자
    BossSkeleton(float startX, float startY, LPD3DXSPRITE spr);
    ~BossSkeleton();
    RECT m_rc;
    // 상태 업데이트 및 렌더링
    void Update(float deltaTime);
    void Draw();
    void SetObstacleRects(const std::vector<RECT>& obstacleRects);
    bool IsAttacking() const { return m_state == BossState::Attack; }
    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    int GetAttackDamage() const { return m_attackDamage; }
    float m_stepSoundTimer = 0.f;
    RECT GetHammerAttackHitbox();
    // 히어로 위치 전달


    //// BossSkeleton 멤버 변수
    //float m_introTimer = 0.0f;
    //const float m_introDuration = 3.0f; // 3초간 연출



    void SetPlayerPos(float x, float y) {
        // 보스 AI가 히어로 위치 추적할 때 사용
        m_targetX = x;
        m_targetY = y;
    }
    void SetHeroCollisionBox(const RECT& rc) {
        m_heroRc = rc;
    }
    bool m_dropCreated = false;  // 드롭 중복 방지용
    bool isHitEffectActive = false;
    // ==============================
// 위치 관련 멤버 변수 추가
// ==============================
    RECT GetAttackHitbox() const;

    std::vector<LPDIRECT3DTEXTURE9> hammerFramesAll;
    std::vector<LPDIRECT3DTEXTURE9> swordFramesAll;

    // 상태 전이
    void ChangeState(BossState newState);

    // 데미지 처리
    void TakeDamage(int dmg);

    // 충돌 감지 박스 갱신
    void UpdateAttackRangeBox();
    BossAttackPattern GetCurrentAttackPattern() const { return m_currentAttackPattern; }
    // 보스 체력 정보
    int m_bossHp = 500;
    int m_bossMaxHp = 500;
    int GetBossHP() const { return m_bossHp; }
    int GetBossMaxHP() const { return m_bossMaxHp; }
    bool m_hasEnteredPhase2 = false;
    void UpdateAttackHitbox();
    int GetHp() const { return m_bossHp; }
    int GetMaxHp() const { return m_bossMaxHp; }

    // 보스 상태
    float m_attackRange = 150.f;
    bool m_isAttacking = false;
    RECT m_bossRc;           // 보스 본체 히트박스
    RECT m_bossAttackRc;     // 보스 공격 사전 범위
    RECT GetCollisionBox() const;
    RECT GetAttackRangeBox() const;
    bool IsDead() const { return m_state == BossState::Dead; }
    // 히어로 설정 함수
    void SetHero(Hero* hero) { m_hero = hero; }

    // 애니메이션 로딩 함수들
    void LoadBossMoveFrames();
    void LoadBossHammerFrames();
    void LoadBossSwordFrames();
};