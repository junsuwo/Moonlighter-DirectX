#include "Collider.h"
#include "Monster.h"
#include "BossSkeleton.h"
#include "Hero.h"
#include <string>
#include <d3dx9tex.h>
#include <random>
#include "Global.h"
#include "Game.h"
#include "SoundManager.h"

extern SoundManager soundManager;
extern Game game;


// 외부 디바이스 참조 (전역에서 제공되어야 함)



const float OFFSET_X = 0.0f;  // 이미지 중심 → 몸 중심까지 x축 차이
const float OFFSET_Y = 0.0f;   // 필요 시 y축도 조절


Direction GetRandomDirection() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 3);
    return static_cast<Direction>(dis(gen));
}

Monster::Monster(float startX, float startY, LPD3DXSPRITE spr)
    : x(startX), y(startY), sprite(spr),
    hp(100), maxHp(100), moveSpeed(600.0f),
    animFrame(0), animTimer(0.0f), animSpeed(60.0f),
    attackCooldown(1.5f), attackTimer(0.0f),
    hitTimer(0.0f), isInvincible(false),
    state(MonsterState::Idle), direction(Direction::Left),
    playerX(0), playerY(0), texture(nullptr),
    attackAnimFrame(0), attackAnimTimer(0.0f), attackAnimSpeed(20.0f)
{
    LoadAnimationFrames(); // 기존 이동애니 메이션 로드
    LoadAttackFrames();       // 신규 공격 애니메이션 로드

    UpdateCollisionBox();      // 초기 충돌박스 세팅
    UpdateAttackRangeBox();    // 초기 공격범위 세팅


    // 체력 초기값 출력 (디버그용)
    char buf[128];
    sprintf(buf, "[Monster] 생성자 호출 - hp: %d, maxHp: %d\n", hp, maxHp);
    OutputDebugStringA(buf);

}

Monster::~Monster() {
    for (auto tex : animFrames) {
        if (tex) tex->Release();
    }
    animFrames.clear();
}

void Monster::LoadAnimationFrames() {
    animFrames.resize(32); // 총 32프레임
    for (int i = 0; i < 32; ++i) {
        char filename[128];
        sprintf(filename, "./resource/Img/monster/GolemMove%03d.png", i); // GolemMove000~031.png
        D3DXCreateTextureFromFileExA(
            g_pd3dDevice,
            filename,
            D3DX_DEFAULT, D3DX_DEFAULT,
            D3DX_DEFAULT, 0,
            D3DFMT_UNKNOWN,
            D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT,
            D3DCOLOR_XRGB(255, 0, 255), // 마젠타 투명색
            nullptr, nullptr,
            &animFrames[i]
        );
    }
}



void Monster::Update(float deltaTime) {
    // HP바 타이머 감소
    if (m_showHpTimer > 0.0f) {
        m_showHpTimer -= deltaTime;
        if (m_showHpTimer <= 0.0f) {
            m_showHpTimer = 0.0f;
           /* OutputDebugStringA("[Monster] HP바 숨김\n");*/
        }
        else {
            /*char buf[128];
            sprintf(buf, "HP바 표시중: 타이머 = %.2f\n", m_showHpTimer);
            OutputDebugStringA(buf);*/
        }
    }
    // 피격 이펙트 유지 시간 처리 (예: 0.2초 동안 빨간색 깜박임)
    if (isHitEffectActive) {
        hitEffectTimer += deltaTime;
        if (hitEffectTimer >= 0.2f) {
            isHitEffectActive = false;
            hitEffectTimer = 0.0f;
        }
    }


    // Hero가 죽었으면 Idle 상태로 바꾸고, 애니메이션 갱신 후 바로 리턴
    if (m_hero && m_hero->IsDead()) {
        if (state != MonsterState::Idle) {
            state = MonsterState::Idle;
            ResetAnimationFrame();
        }
        if (m_firstEnterDungeon) {
            ShowHpBarOnDungeonEnter();
        }  // 던전 진입 시 HP바 잠깐 표시

        UpdateAnimation(deltaTime);
        UpdateCollisionBox();
        UpdateAttackRangeBox();
        UpdateDetectRangeBox();
        return;
    }

    // 무적 시간 업데이트 (히트 이펙트 유지 및 해제)
    if (isInvincible) {
        hitTimer += deltaTime;
        if (hitTimer >= hitDuration) {
            isInvincible = false;
            hitTimer = 0.0f;

            if (m_pendingHit) {
                m_pendingHit = false;
            }
        }
    }

    // 상태 업데이트
    UpdateState(deltaTime);
   
    // 애니메이션 갱신
    UpdateAnimation(deltaTime);

    // 공격 상태면 공격 애니메이션 추가 갱신
    if (state == MonsterState::Attack) {
        UpdateAttackAnimation(deltaTime);
    }

    // 충돌 및 시야 박스 갱신
    UpdateCollisionBox();
    UpdateAttackRangeBox();
    UpdateDetectRangeBox();
   
}

void Monster::Draw() {
    if (state == MonsterState::Dead) return;  // ✅ 사망 상태면 바로 리턴해서 아무것도 안 그림

    if (state == MonsterState::Attack) {
        DrawAttack();  // 공격 애니메이션 그리기
        // return 하지 않고 계속 진행해서 HP바도 그리기
    }
    else {
        // 이동 / 대기 애니메이션 그리기
        int baseIndex = 0;
        switch (direction) {
        case Direction::Left:  baseIndex = 0; break;
        case Direction::Right: baseIndex = 8; break;
        case Direction::Up:    baseIndex = 16; break;
        case Direction::Down:  baseIndex = 24; break;
        }

        int frameIndex = baseIndex + animFrame;
        if (frameIndex < 0 || frameIndex >= (int)animFrames.size()) return;

        LPDIRECT3DTEXTURE9 tex = animFrames[frameIndex];
        if (!tex) return;

        D3DXVECTOR2 scale(0.8f, 0.8f);
        D3DXVECTOR2 center(FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f);
        D3DXVECTOR2 position(x, y);

        D3DXMATRIX mat;
        D3DXMatrixTransformation2D(
            &mat,
            nullptr, 0.0f,
            &scale,
            &center,
            0.0f,
            &position
        );

        sprite->Begin(D3DXSPRITE_ALPHABLEND);
        sprite->SetTransform(&mat);

        D3DCOLOR drawColor;
        if (isHitEffectActive) {
            drawColor = D3DCOLOR_ARGB(200, 255, 100, 100);  // 피격 빨간 효과 우선
        }
        else if (isInvincible) {
            drawColor = D3DCOLOR_ARGB(160, 255, 100, 100);  // 무적일 때 약간 다른 빨간색
        }
        else {
            drawColor = D3DCOLOR_XRGB(255, 255, 255);       // 정상 색상
        }


        sprite->Draw(tex, nullptr, nullptr, nullptr, drawColor);
        sprite->End();

        D3DXMATRIX identity;
        D3DXMatrixIdentity(&identity);
        sprite->SetTransform(&identity);
    }

    // HP 바 그리기: 공격 중이거나 타이머가 남아있으면 표시
   
    if (m_showHpTimer > 0.0f) {
        const int BAR_WIDTH = 60;
        const int BAR_HEIGHT = 8;
        const int offsetY = 30;
        const int offsetX = 75;  // 필요하면 조절 가능

        float hpRatio = static_cast<float>(hp) / maxHp;
        int barX = static_cast<int>(x + offsetX);
        int barY = static_cast<int>(y + offsetY);

        coll.FillRect(barX, barY, BAR_WIDTH, BAR_HEIGHT, D3DCOLOR_ARGB(180, 0, 0, 0));
        coll.FillRect(barX, barY, static_cast<int>(BAR_WIDTH * hpRatio), BAR_HEIGHT, D3DCOLOR_ARGB(200, 255, 0, 0));
    }

    // 디버그 박스 그리기
    if (g_showColliderBox) {
        coll.BoxSow(attackRangeRc, 0, 0, 0x8800ff00);
        coll.BoxSow(m_rc, 0, 0, 0xffff0000);
        coll.BoxSow(attackRangeRc, 0, 0, 0xff00ff00);
        coll.BoxSow(detectRangeRc, 0, 0, D3DCOLOR_ARGB(255, 0, 255, 255));
    }
}

void Monster::UpdateAnimation(float deltaTime) {
    animTimer += deltaTime;
    if (animTimer >= 1.0f / animSpeed) {
        animTimer = 0.0f;
        animFrame = (animFrame + 1) % 8; // 방향별 프레임 수는 8개
    }
}

void Monster::UpdateState(float deltaTime) {
   /* char buf[128];
    sprintf(buf, "[Monster] 상태: %d\n", (int)state);
    OutputDebugStringA(buf);*/
    const float ATTACK_ENTER_DISTANCE = 190.0f;  // ★ 공격 진입 거리 설정
    switch (state) {
    case MonsterState::Idle:
        // 시야 범위 박스 기준으로 플레이어 좌표 비교
        if (PtInRect(&detectRangeRc, POINT{ (LONG)playerX, (LONG)playerY })) {
            ChangeState(MonsterState::Chase);
        }
        break;

    case MonsterState::Chase: {
        // ★★★ 공격 모션 진행 중이면 이동/상태 변경 금지 ★★★
        if (m_isAttackStarted) break;
        UpdateMovement(deltaTime);  // ① 이동 먼저

        float dist = DistanceToPlayer();

        
        
           // 공격 상태가 아니고, 공격 중이 아니고, 쿨다운 끝났을 때만 공격 진입
        if (dist <= ATTACK_ENTER_DISTANCE) {
            ChangeState(MonsterState::Attack);
            attackTimer = 0.0f;
        }

        break;
    }


    case MonsterState::Attack:
        attackTimer += deltaTime;

        // 공격 애니메이션 갱신
        UpdateAttackAnimation(deltaTime);

        // ✅ 공격이 아직 진행 중이면 상태 전환하지 않음
        if (m_isAttackStarted)
            break;

        // ✅ 공격이 끝났고, 거리에 따라 상태 전환
        if (DistanceToPlayer() > ATTACK_ENTER_DISTANCE) {
            ChangeState(MonsterState::Chase);  // 다시 추적
        }
        else {
            ChangeState(MonsterState::Idle);   // 공격 후 대기
        }
        break;

    case MonsterState::Hit:

        hitTimer += deltaTime;
        if (hitTimer >= hitDuration) {
            soundManager.PlaySFX("golemHit");
            hitTimer = 0.0f;
            isInvincible = false;
            ChangeState(MonsterState::Idle);
        }
        break;

    case MonsterState::Dead:
        if (deadSoundPlayed == false) {
            soundManager.PlaySFX("golemDie");
            deadSoundPlayed = true;
        }
        // 사망 처리 (애니메이션 재생 등)
        break;
    }
}

void Monster::UpdateMovement(float deltaTime) {

    float dx = playerX - x;
    float dy = playerY - y;

    float dist = sqrtf(dx * dx + dy * dy);
    if (dist > 1.0f) {
        dx /= dist;
        dy /= dist;

        float nextX = x + dx * moveSpeed * deltaTime;
        float nextY = y + dy * moveSpeed * deltaTime;

        // ⭐️ 충돌 박스 절반 크기 계산
        const float scale = 0.8f;
        int monsterW = (int)(190 * scale);
        int monsterH = (int)(190 * scale);
        int halfW = monsterW / 2;
        int halfH = monsterH / 2;

        // ⭐️ 히어로와의 거리 유지
        float nextDist = sqrtf((playerX - nextX) * (playerX - nextX) +
            (playerY - nextY) * (playerY - nextY));
        const float MIN_DISTANCE = 30.0f;

        // ⭐️ 1. 히어로와 너무 가까워지지 않음
        bool isTooCloseToHero = false;
        RECT dummy{};
        if (IntersectRect(&dummy, &m_rc, &m_heroRc)) {
            isTooCloseToHero = true;
        }

        // ⭐️ 2. 맵 경계 내에 있는지 확인
        bool isInsideMap =
            nextX - halfW >= g_mapBoundary.left &&
            nextX + halfW <= g_mapBoundary.right &&
            nextY - halfH >= g_mapBoundary.top &&
            nextY + halfH <= g_mapBoundary.bottom;

        // ⭐️ 3. 문/장애물과 충돌하지 않는지 확인
        bool isCollidingWithObstacle = false;
        RECT nextRc = {
            (LONG)(nextX - halfW),
            (LONG)(nextY - halfH),
            (LONG)(nextX + halfW),
            (LONG)(nextY + halfH)
        };

        for (const auto& obs : m_obstacleRects) {
            RECT overlap{};
            if (IntersectRect(&overlap, &nextRc, &obs)) {
                isCollidingWithObstacle = true;
                break;
            }
        }

        // ⭐️ 이동 조건 통과 시만 위치 갱신
        if (!isTooCloseToHero  && !isCollidingWithObstacle && nextDist > MIN_DISTANCE) {
            x = nextX;
            y = nextY;

            // 방향 판정
            if (fabs(dx) > fabs(dy)) {
                direction = dx < 0 ? Direction::Left : Direction::Right;
            }
            else {
                direction = dy < 0 ? Direction::Up : Direction::Down;
            }
        }
    }
}
// 맵 경계 및 장애물 충돌 검사 함수
bool Monster::IsCollidingWithMapOrObstacles(const RECT& rc) {
    extern RECT g_mapBoundary;  // 전역 맵 경계

    // 맵 경계 밖으로 나가는지 검사
    if (rc.left < g_mapBoundary.left || rc.right > g_mapBoundary.right ||
        rc.top < g_mapBoundary.top || rc.bottom > g_mapBoundary.bottom) {
        return true; // 맵 밖 충돌
    }

    // 장애물 박스와 충돌 검사
    for (const RECT& obst : m_obstacleRects) {
        if (IntersectRect(nullptr, &rc, &obst)) {
            return true;
        }
    }

    return false; // 충돌 없음
}
void Monster::UpdateAttack(float deltaTime) {
    attackTimer += deltaTime;
    if (attackTimer >= attackCooldown) {
        PerformAttack();
        attackTimer = 0.0f;
    }

    // 플레이어 멀어지면 다시 추적
    if (DistanceToPlayer() > 80.0f) {
        ChangeState(MonsterState::Chase);
    }
}

void Monster::ChangeState(MonsterState newState) {
    if (state == newState) return;  // 상태가 같으면 무시

    state = newState;
    animFrame = 0;
    animTimer = 0.0f;

    if (newState == MonsterState::Dead) {
        isInvincible = false;
        m_isAttackStarted = false;
        deadSoundPlayed = false;
        // 추가로 필요한 죽음 처리 (이동 멈춤 등) 여기서
    }

    if (newState == MonsterState::Attack) {
        attackAnimFrame = 0;
        attackAnimTimer = 0.0f;
        m_hasDealtDamageThisAttack = false; // ★ 공격 시작 시 초기화
        m_isAttackStarted = true;   // 공격 상태 시작 표시
    }
    else {
        m_isAttackStarted = false;  // 공격 상태 벗어나면 표시 해제
    }

    // 무적 상태 해제
    if (newState != MonsterState::Hit) {
        isInvincible = false;
        hitTimer = 0.0f;
    }
}



void Monster::PerformAttack() {
    {


        if (!m_hero || m_hero->IsInvincible())
            return;

        soundManager.PlaySFX("golemAttack");
        m_hasDealtDamageThisAttack = false;  // 새로운 공격 시작 시 데미지 처리 플래그 초기화

    }
}

void Monster::TakeDamage(int damage) {
    if (IsDead()) return;  // 무적 중이거나 이미 죽었으면 무시
    OutputDebugStringA("[Monster] TakeDamage 호출\n");
    
   hp -= damage;
    if (hp <= 0) {
        hp = 0;
        
        ChangeState(MonsterState::Dead);
        
        return;
    }
    

    m_showHpTimer = 0.5f; // HP바 표시시간

    // 무적 상태 없애고 피격 이펙트만 처리
    isInvincible = false;
    hitTimer = 0.0f;
    isHitEffectActive = true;  // 피격 이펙트 시작

    if (state == MonsterState::Attack) {
        m_pendingHit = true;  // 공격 중 피격 여부 기록
        // 상태 전환 없음! 공격 모션 유지
    }
    else {
        ChangeState(MonsterState::Hit);
    }
}

bool Monster::IsDead() const {
    return state == MonsterState::Dead || hp <= 0;
}

void Monster::SetPlayerPos(float px, float py) {
    playerX = px;
    playerY = py;
}

float Monster::DistanceToPlayer() const {
    float dx = playerX - x;
    float dy = playerY - y;
    return sqrtf(dx * dx + dy * dy);
}


// ** 충돌 박스 갱신 **
void Monster::UpdateCollisionBox() {
    float scale = 0.8f;
    int monsterW = (int)(190 * scale);
    int monsterH = (int)(190 * scale);

    int halfW = monsterW / 2;
    int halfH = monsterH / 2;

    // offset 0 으로 기본 테스트
    float offsetX = 85.0f;
    float offsetY = 90.0f;

    // 박스 크기 조절용 추가 오프셋
    int shrinkW = 50;  // ←→ 10픽셀씩 줄어듦
    int shrinkH = 10;  // ↑↓ 15픽셀씩 줄어듦

    m_rc.left = (LONG)(x - halfW + offsetX + shrinkW / 2-20);
    m_rc.right = (LONG)(x + halfW + offsetX - shrinkW / 2+30);
    m_rc.top = (LONG)(y - halfH + offsetY + shrinkH / 2+15);
    m_rc.bottom = (LONG)(y + halfH + offsetY - shrinkH / 2+30);

    //// 디버그 출력용 버퍼
    //char buffer[128];
    //sprintf(buffer, "Monster Pos: (%.1f, %.1f) | CollisionBox: (L:%ld, T:%ld, R:%ld, B:%ld)\n",
    //    x, y, m_rc.left, m_rc.top, m_rc.right, m_rc.bottom);
    //OutputDebugStringA(buffer);
    }


// ★ 시야 범위 박스 갱신
void Monster::UpdateDetectRangeBox() {
    const int RANGE = 170;
    const int halfWidth = 190 / 2;  // 몬스터 폭 절반
    const int halfHeight = 190 / 2; // 몬스터 높이 절반

    detectRangeRc.left = (LONG)(x + halfWidth - RANGE);
    detectRangeRc.top = (LONG)(y + halfHeight - RANGE);
    detectRangeRc.right = (LONG)(x + halfWidth + RANGE);
    detectRangeRc.bottom = (LONG)(y + halfHeight + RANGE);
}


// ** 공격 범위 박스 갱신 **
void Monster::UpdateAttackRangeBox() {
    int baseW = m_rc.right - m_rc.left;
    int baseH = m_rc.bottom - m_rc.top;

    int rangeW = baseW /2;  // 공격 범위 폭 확장
    int rangeH = baseH /2 ;

    const int offset = 10;    // 몸 

    switch (direction) {
    case Direction::Left:
        attackRangeRc.left = m_rc.left - rangeW;
        attackRangeRc.right = m_rc.left;
        attackRangeRc.top = m_rc.top + 10;      // 위 아래 살짝 자름
        attackRangeRc.bottom = m_rc.bottom - 10;
        break;

    case Direction::Right:
        attackRangeRc.left = m_rc.right;
        attackRangeRc.right = m_rc.right + rangeW;
        attackRangeRc.top = m_rc.top + 10;
        attackRangeRc.bottom = m_rc.bottom - 10;
        break;

    case Direction::Up:
        attackRangeRc.left = m_rc.left + 10;
        attackRangeRc.right = m_rc.right - 10;
        attackRangeRc.top = m_rc.top - rangeH;
        attackRangeRc.bottom = m_rc.top;
        break;

    case Direction::Down:
        attackRangeRc.left = m_rc.left + 10;
        attackRangeRc.right = m_rc.right - 10;
        attackRangeRc.top = m_rc.bottom;
        attackRangeRc.bottom = m_rc.bottom + rangeH;
        break;

    default:
        attackRangeRc = m_rc;
        break;
    }
}

void Monster::SetHeroCollisionBox(const RECT& rc) {
    m_heroRc = rc; // 
}

void Monster::SetObstacleRects(const std::vector<RECT>& v) {
    m_obstacleRects = v; //
}


// 1) 공격 애니메이션 텍스처 로드 함수 구현
void Monster::LoadAttackFrames() {
    attackFrames.resize(52); // 총 52장 (13장 x 4방향)
    for (int i = 0; i < 52; ++i) {
        char filename[128];
        sprintf(filename, "./resource/Img/monster/attack/Golemattack%03d.png", i);
        HRESULT hr = D3DXCreateTextureFromFileExA(
            g_pd3dDevice,
            filename,
            D3DX_DEFAULT, D3DX_DEFAULT,
            D3DX_DEFAULT, 0,
            D3DFMT_UNKNOWN,
            D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT,
            D3DCOLOR_XRGB(255, 0, 255), // 투명색 (마젠타)
            nullptr, nullptr,
            &attackFrames[i]
        );
        if (FAILED(hr)) {
            // 실패 처리 (디버그용)GetCollisionBox
            OutputDebugStringA("[Monster] 공격 텍스처 로드 실패\n");
            attackFrames[i] = nullptr;
        }
    }
}

// 2) 공격 애니메이션 업데이트 함수
void Monster::UpdateAttackAnimation(float deltaTime) {
    attackAnimTimer += deltaTime;

    if (attackAnimTimer >= 1.0f / attackAnimSpeed) {
        attackAnimTimer = 0.0f;

        attackAnimFrame += 3;

  
        // 공격 유효 프레임 조건 내에서 한 번만 데미지 처리
        if (attackAnimFrame >= 11 && attackAnimFrame <= 12) {
            if (!m_hasDealtDamageThisAttack) {
                RECT heroRc = m_hero->GetCollisionBox();  // Hero 포인터를 이용해 가져오기
                RECT overlap;
                if (IntersectRect(&overlap, &attackRangeRc, &heroRc)) {
                    if (!m_hero->IsInvincible()) {
                        m_hero->TakeDamage(10);
                        OutputDebugStringA("[Monster] Hero 피격!\n");
                        m_hasDealtDamageThisAttack = true;  // 한 번 데미지 주면 true 설정
                         // 여기서 데미지 텍스트 추가
                D3DXVECTOR2 dmgPos(m_hero->GetX()+20, m_hero->GetY() - 30);
                Game::GetInstance()->GetDamageTextManager().AddText(dmgPos, 10);
                // 사운드 재생
                soundManager.PlaySFX("golemAttack");
                    }
                }
            }
        }

        // 애니메이션 종료 시 초기화 및 상태 전환
        if (attackAnimFrame >= 13) {
            attackAnimFrame = 0;
            m_isAttackStarted = false;
            m_hasDealtDamageThisAttack = false;  // 다음 공격 위해 반드시 초기화
            ChangeState(MonsterState::Idle);
        }
    }
}

// 3) 공격 애니메이션 그리기 함수
void Monster::DrawAttack() {
    if (state != MonsterState::Attack) return;

    // 방향별 시작 인덱스 지정 (0: Left, 13: Right, 26: Up, 39: Down)
    int baseIndex = 0;
    switch (direction) {
    case Direction::Left:  baseIndex = 0; break;
    case Direction::Right: baseIndex = 13; break;
    case Direction::Up:    baseIndex = 26; break;
    case Direction::Down:  baseIndex = 39; break;
    }
    int frameIndex = baseIndex + attackAnimFrame;

    if (frameIndex < 0 || frameIndex >= (int)attackFrames.size()) {
        /*OutputDebugStringA("[Monster] 공격 애니메이션 프레임 범위 초과\n");*/
        return;
    }

    LPDIRECT3DTEXTURE9 tex = attackFrames[frameIndex];  // <- frameIndex로 바꿔야 함
    if (!tex) {
        /*OutputDebugStringA("[Monster] 공격 텍스처 null\n");*/
        return;
    }

    // 기존 Draw()와 동일한 방식으로 그리기
    D3DXVECTOR2 scale(0.8f, 0.8f);
    D3DXVECTOR2 center(FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f);
    D3DXVECTOR2 position(x, y);

    D3DXMATRIX mat;
    D3DXMatrixTransformation2D(
        &mat,
        nullptr, 0.0f,
        &scale,
        &center,
        0.0f,
        &position
    );

    sprite->Begin(D3DXSPRITE_ALPHABLEND);
    sprite->SetTransform(&mat);

    // 무적 시 빨간 빛 표시(필요 시)
    D3DCOLOR drawColor = isHitEffectActive
        ? D3DCOLOR_ARGB(160, 255, 100, 100)
        : D3DCOLOR_XRGB(255, 255, 255);

    sprite->Draw(tex, nullptr, nullptr, nullptr, drawColor);

    sprite->End();

    // 트랜스폼 복원
    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    sprite->SetTransform(&identity);
  /*  OutputDebugStringA("[Monster] 공격 애니메이션 그리는 중\n");*/

    if (g_showColliderBox) {
        // 1) 공격 범위 박스 (반투명 초록)
        coll.BoxSow(attackRangeRc, 0, 0, 0x8800ff00);

        // 2) 충돌 박스 (빨강)
        coll.BoxSow(m_rc, 0, 0, 0xffff0000);

        // 3) 공격 범위 박스 (불투명 초록)
        coll.BoxSow(attackRangeRc, 0, 0, 0xff00ff00);

        // 4) 시야 범위 박스 (시안)
        coll.BoxSow(detectRangeRc, 0, 0, D3DCOLOR_ARGB(255, 0, 255, 255));
    }
}


void Monster::ResetAnimationFrame()
{
    animFrame = 0;    // 현재 애니메이션 프레임 초기화
    animTimer = 0.f;  // 애니메이션 타이머 초기화
}

void Monster::ShowHpBarOnDungeonEnter() {
    if (m_firstEnterDungeon) {
        m_showHpTimer = 0.05f;  // 0.5초간 HP바 표시
        m_firstEnterDungeon = false;
    }
}
