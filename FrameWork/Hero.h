#pragma once
#include "Include.h"
#include <array>

#ifdef LEFT
#undef LEFT
#endif

#ifdef RIGHT
#undef RIGHT
#endif

#ifdef UP
#undef UP
#endif

#ifdef DOWN
#undef DOWN
#endif



class BossSkeleton;

// 하나의 애니메이션 구간 정보
struct AnimInfo {
    int rowStart;        // 시트에서 시작 행(0‑based)
    int rowCount;        // 행 수
    int framesPerRow;    // 열 수
    bool loop;           // 마지막 프레임 이후 다시 0으로?
    int totalFrames() const { return rowCount * framesPerRow; }
};

class Hero {
public:
    // 애니메이션 종류 열거형 (걷기,구르기,대기,죽음)
    enum class Anim {
        WALK = 0,
        ROLL = 1,
        IDLE = 2,
        DIE = 3,
        ATTACK = 4
    };

    /* ★ 방향 열거형 추가 ─────────────────── */
    // 충돌 피하기 위해 방향 이름에 접두어 DIR_ 추가
    enum class Direction {
        DIR_LEFT = 0,
        DIR_RIGHT = 1,
        DIR_UP = 2,
        DIR_DOWN = 3,

    };
    /* ─────────────────────────────────────── */

    Hero();
    ~Hero();


    int m_hp;
    int m_maxHP; // 최대 HP 추가 (추천);
    void Heal(int amount);     // ✅ 체력 회복 함수
    int GetHp() const { return m_hp; }
    float m_invincibleTimer;
    bool m_isDead;
    bool IsInvincible() const;

    float hitTimer = 0.0f;     // 무적 타이머
    const float hitDuration = 0.3f; // 무적 지속 시간 (초)

    void Hero::TakeDamage(int damage);
 
    RECT Hero::GetCollisionBox() const;
   
    static constexpr int FRAME_WIDTH = 120;
    static constexpr int FRAME_HEIGHT = 120;
    float walkSoundTimer = 0.f;  // 걷기 사운드 쿨타임 타이머
    ULONGLONG m_lastAttackSoundTime = 0;  // 마지막으로 소리를 낸 시간
    const ULONGLONG ATTACK_SOUND_COOLDOWN = 150; // ms 단위 쿨타임
    bool IsDead() const;

    void Hero::UpdateCollisionBox();

    bool Init(const char* pngPath);
    void SetAnim(Anim newAnim, bool forceRestart = false);
    void Update();
    void Draw();
    void Move(float dx, float dy) { m_x += dx; m_y += dy; }
    void DrawRollEffect();  // 구르기 이펙트 출력 함수
    BossSkeleton* boss = nullptr;
  
    RECT m_rc; // 충돌 박스 (AABB)

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }
    void SetX(float x) { m_x = x; }
    void SetY(float y) { m_y = y; }
    void SetPos(float x, float y) { m_x = x; m_y = y; }

    // ✅ 중심 좌표 반환 함수 추가
    float GetCenterX() const { return m_x; }
    float GetCenterY() const { return m_y; }

    /* ★ 방향 getter / setter */
    void SetDirection(Direction dir); // 이동 시 방향 설정
    Direction GetDirection() const { return m_dir; }
    Anim GetCurAnim() const { return m_curAnim; }
    static Direction GetIdleDirectionFromMove(Direction moveDir);

    bool IsRolling() const { return m_isRolling; }
    void StartRoll();

    // 구르기 이펙트
    int m_rollEffectFrame = 0;
    ULONGLONG m_rollEffectTime = 0;
    const int ROLL_EFFECT_FRAME_COUNT = 5;
    const int ROLL_EFFECT_FRAME_DELAY = 80; // 0.05초 간격

    // 어택 관련
    int m_attackStage = 0;       // 공격 단계: 1~3
    ULONGLONG m_lastAttackTime = 0;
    const int ATTACK_INPUT_DELAY = 300;  // 300ms 안에 J키 누르면 다음 콤보
    ULONGLONG m_attackAnimStartTime = 0;  // 공격 애니메이션 시작 시간 저장
    bool Hero::IsAttacking() const;
    bool m_comboWindowActive = false;   // 콤보 입력 가능한 시간
    ULONGLONG m_comboWindowStartTime = 0;
    RECT GetAttackHitbox() const; // 히트박스
   
    bool m_hasDealtDamageStage1 = false;
    bool m_hasDealtDamageStage2 = false;

private:

    


    void advanceFrame();           // 내부 프레임 진행
    void calcSrcRect(RECT& rc);    // 현재 프레임의 소스 RECT 계산

    Sprite m_sprite;         // 기본 캐릭터 스프라이트
    std::array<Sprite, 44> m_attackFrames;  // 44장의 공격 프레임

    float       m_x{}, m_y{};

    Sprite m_rollEffect;         // 구르기 효과
    Sprite m_rollEffectSprite;     // 이펙트 스프라이트
    bool m_isInvincible = false;
    ULONGLONG m_invincibleEndTime = 0;

    // 피격
    ULONGLONG m_lastHitTime = 0; // 최근 피격 시간
    const ULONGLONG DAMAGE_COLOR_DURATION = 200; // 0.2초간 빨간색 유지
    bool m_isInvincibleByHit = false;     // 공격 맞고 무적
    bool m_isInvincibleByRoll = false;    // 구르기 중 무적

    Anim        m_curAnim{ Anim::IDLE };
    Direction   m_dir{ Direction::DIR_DOWN };   // ★ 기본 방향 ↓
    Direction m_attackDir;   // 새로 추가할 공격 방향
   
    int         m_frame{};
    ULONGLONG   m_animTime{};

    //몬스터 관련 멤버
    bool m_hasDealtDamageThisAttack = false;

    static const ULONGLONG FRAME_MS = 50;   // 70 ms ≒ 14 FPS
    static const AnimInfo animTable[5];

    // 구르기 관련 멤버
    bool        m_isRolling{ false };
    float       m_rollDistance{ 0.f };
    const float m_rollMaxDistance{ 200.0f };
    const float m_rollSpeed{ 5.0f };

};

extern Hero hero;   // 필요하면 사용