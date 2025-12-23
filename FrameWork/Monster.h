#pragma once
class Hero;
#include <Windows.h>
#include <d3dx9.h>
#include <vector>


extern RECT g_mapBoundary;
// 몬스터의 상태를 나타내는 열거형 (FSM)
enum class MonsterState {
    Idle,   // 대기 상태 (플레이어를 기다림)
    Chase,  // 플레이어 추적 상태 (플레이어 쪽으로 이동)
    Attack, // 공격 상태 (공격 애니메이션 실행 및 판정)
    Hit,    // 피격 상태 (무적 시간 포함)
    Dead    // 사망 상태 (죽음 처리)
};

// 몬스터가 바라보는 방향 (애니메이션 프레임 선택용)
enum class Direction {
    Left = 0,
    Right = 1,
    Up = 2,
    Down = 3
};

class Monster {
protected:
    // 위치 (중심 좌표 기준)
    float x, y;
    float m_x, m_y;
    RECT detectRangeRc;  // 시야 범위 박스
    LPDIRECT3DTEXTURE9 texture;     // 더 이상 사용 안 함 (단일 시트 방식일 때만 사용)
    virtual void UpdateDetectRangeBox();  // 시야 범위 박스 갱신 함수 선언 
    // ** 충돌박스: 몬스터 위치 기준 박스 **
    RECT m_rc;   // ** 몬스터 본체 충돌 박스 **

    // 공격 상태일 때 공격 처리 (쿨다운 체크)
    virtual void UpdateAttack(float deltaTime);

    Hero* m_hero = nullptr;
    // ** 공격 범위 박스: 공격 판정용 영역 **
    RECT attackRangeRc;  // ** 몬스터 공격 범위 충돌 박스 **
    // 현재 상태 (FSM)
    MonsterState state;
    // 바라보는 방향 (애니메이션 행 선택용)
    Direction direction;



private:
    
    // 체력 및 최대 체력
    int hp;
    int maxHp;

   


    // 이동 속도 (픽셀/초)
    float moveSpeed;

   

   

    // 공격 쿨다운 시간 및 현재 쿨다운 타이머
    float attackCooldown=0.05f;    // 공격 간 최소 간격 (초)
    float attackTimer;       // 마지막 공격 이후 경과 시간 (초)

    // 애니메이션 관련 변수들
    int animFrame;           // 현재 프레임 인덱스 (0~7)
    float animTimer;         // 프레임 갱신용 타이머 (초)
    float animSpeed;         // 초당 프레임 전환 속도 (fps)

    // 피격 시 무적 시간 타이머
    float hitTimer;          // 무적 경과 시간 (초)
    const float hitDuration = 0.0f;  // 무적 지속 시간 (초)
    bool isInvincible;       // 무적 여부 플래그

    // 플레이어 위치 (추적용)
    float playerX, playerY;



    // 몬스터 스프라이트 관련 (외부에서 주입받음)
    
    LPD3DXSPRITE sprite;           // 스프라이트 렌더러 (외부 소유)

    // 스프라이트 한 프레임 크기 (픽셀)
    static const int FRAME_WIDTH = 190;
    static const int FRAME_HEIGHT = 190;


    static constexpr int COLLIDE_W = 60;
    static constexpr int COLLIDE_H = 80;
    static constexpr int ATTACK_W = COLLIDE_W / 2;
    static constexpr int ATTACK_H = COLLIDE_H;
    // 개별 프레임 텍스처 (GolemMove000~031.png)
    std::vector<LPDIRECT3DTEXTURE9> animFrames;

 


   

    bool m_hasDealtDamageThisAttack = false; // 아이템 드롭 여부 체크용
    bool m_hasDroppedItem = false;

    const float HP_BAR_SHOW_DURATION = 2.0f; // HP바가 보여지는 시간 (초)
    bool m_firstEnterDungeon = true;        // 처음 입장했는지 여부
    std::vector<RECT> m_obstacleRects;  // 맵 내 장애물(문 등) 박스 리스트
    RECT m_heroRc;                      // 히어로 충돌 박스 (업데이트마다 갱신)
    // 피격 시 빨간색 효과용 플래그와 타이머
    bool isHitEffectActive = false;
    float hitEffectTimer = 0.0f;
    const float hitEffectDuration = 0.3f;  // 피격 빨간 효과 지속시간 (초)

    float m_showHpTimer = 0.0f;       // HP바 표시 시간 남은 초
    const float HP_BAR_DISPLAY_TIME = 2.0f; // HP바 표시 유지 시간 (초)


    // UpdateMovement 시그니처 변경 (deltaTime와 충돌 관련 인자 제거하고 내부 참조 활용)
    void UpdateMovement(float deltaTime);

    // 맵 및 장애물 충돌 체크 함수
    bool IsCollidingWithMapOrObstacles(const RECT& rc);


    // 상태별 동작 처리 (Idle, Chase, Attack 등)
    void UpdateState(float deltaTime);

    //// 추적 상태일 때 이동 처리
    //void UpdateMovement(float deltaTime);

   

    // 상태 변경 및 애니메이션 초기화
    void ChangeState(MonsterState newState);

    // 실제 공격 판정 처리 (충돌 검사 등)
    void PerformAttack();

    // 애니메이션 프레임 갱신
    void UpdateAnimation(float deltaTime);

    // 플레이어와 몬스터 거리 계산 (추적 조건 등)
    float DistanceToPlayer() const;

    // 애니메이션 프레임 로드 (32장)
    void LoadAnimationFrames();

    // ** 충돌 박스 갱신 (매 프레임 위치에 맞춰서) **
    void UpdateCollisionBox();    // ** 충돌박스 좌표 계산 함수 **

    // ** 공격 범위 박스 갱신 (방향에 따른 범위 조절) **
    void UpdateAttackRangeBox();  // ** 공격 범위 좌표 계산 함수 **

    // ** 디버그용 박스 그리기 함수 **
    void DrawCollisionBoxes();    // ** 충돌박스 및 공격범위박스 시각화 **


public:
    // 생성자: 위치, 스프라이트 주입
    Monster(float startX, float startY, LPD3DXSPRITE spr);

    bool m_dropCreated = false;   // 아이템 드롭 생성 완료 여부 플래그

    // 소멸자 (텍스처는 내부에서 관리하므로 해제)
    ~Monster();

    void SetHero(Hero* hero) { m_hero = hero; }

    // 애니메이션 프레임 0 초기화 함수
    void ResetAnimationFrame();

    // 매 프레임 호출: 상태 업데이트 (이동, 공격 등)
    virtual void Update(float deltaTime);

    // 몬스터 화면 출력
    virtual void Draw();

    float m_scale = 0.8f;
    bool deadSoundPlayed = false;

    // 몬스터가 데미지를 입음
    virtual void TakeDamage(int damage);

    // 몬스터 거리
    float dx = playerX - x;
    float dy = playerY - y;

    // 몬스터 사망 여부 체크
    bool IsDead() const;

    float GetX() const { return x; }
    float GetY() const { return y; }

    // HP 바 관련
    bool m_showHpBar = false;           // 현재 HP바를 보여줄지 여부
    float m_hpBarShowTimer = 0.0f;      // HP바 표시 시간 타이머
    void Monster::ShowHpBarOnDungeonEnter();

    bool m_pendingHit = false;  // 공격 중 맞았음을 기억하는 플래그
    int pendingDamage = 0;      // 나중에 적용할 데미지

    //드롭여부
    bool m_isAttackStarted = false;

    bool HasDroppedItem() const { return m_hasDroppedItem; }
    void SetHasDroppedItem(bool val) { m_hasDroppedItem = val; }

    // 
    // 플레이어 위치 전달 (추적용)
    void SetPlayerPos(float px, float py);
    

    // 게임 메인에서 몬스터에게 장애물 박스와 히어로 박스 전달받도록 함
    void SetHeroCollisionBox(const RECT& rc);           // ✔️ 히어로 박스 설정 함수 선언
    void SetObstacleRects(const std::vector<RECT>& v);  // ✔️ 장애물 리스트 설정 함수 선언

    // 몬스터 충돌박스 가져오기
    RECT GetCollisionBox() const { return m_rc; }


    // 공격 애니메이션 관련 멤버 변수
    std::vector<LPDIRECT3DTEXTURE9> attackFrames;  // 공격 스프라이트 텍스쳐들
    int attackAnimFrame = 0;                       // 현재 공격 애니메이션 프레임 인덱스
    float attackAnimTimer = 0.0f;                  // 애니메이션 타이머 (초)
    float attackAnimSpeed = 160.0f;                 // 공격 애니메이션 재생 속도 (FPS)

    // 공격 애니메이션 로딩 함수
    void LoadAttackFrames();

    // 공격 애니메이션 업데이트 함수
    void UpdateAttackAnimation(float deltaTime);

    // 공격 애니메이션 그리기 함수
    void DrawAttack();

    Hero* GetHero() const { return m_hero; }


    void SetX(float newX) { x = newX; }
    void SetY(float newY) { y = newY; }
    void SetPos(float newX, float newY) { x = newX; y = newY; }


   

};