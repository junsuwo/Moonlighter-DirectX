#include "Hero.h"
#include "UIManager.h"
#include "Global.h"
#include"BossSkeleton.h"
#include "Game.h"
#include "SoundManager.h"

extern SoundManager soundManager;


extern Game game;

extern std::vector<Monster*> monsters;

extern UIManager g_UIManager;



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


/* ===== 애니메이션 테이블 정의 ===== */
const AnimInfo Hero::animTable[5] = {
    /* WALK */ { 0,  4,  8, true  },
    /* ROLL */ { 4,  4,  8, true  },
    /* IDLE */ { 8,  4, 10, true  },
    /* DIE  */ { 12, 1, 10, false },
    /* ATTACK */ { 0,  4, 11, false } };
/* ================================== */




Hero hero;          // 전역 인스턴스 (선택)

Hero::Hero()
    : m_x(500.f), m_y(500.f),
    m_curAnim(Anim::IDLE), m_frame(0), m_animTime(0),
    m_isRolling(false), m_rollDistance(0.f), m_rollMaxDistance(100.f), m_rollSpeed(5.f),
    m_isInvincible(false), m_invincibleEndTime(0),m_hp(100), m_maxHP(100), m_invincibleTimer(0.0f),m_isDead(false)
{
}

Hero::~Hero() {}


/* ===== 초기화 함수 ===== */
bool Hero::Init(const char* pngPath)
{
    bool spriteOk = m_sprite.Create(pngPath, false, D3DCOLOR_XRGB(255, 0, 255));
    bool effectOk = m_rollEffect.Create("./resource/Img/effect/roll_dust.png", true, D3DCOLOR_XRGB(255, 0, 255));
    m_rollEffectSprite.Create("./resource/Img/effect/roll_dust.png", true, D3DCOLOR_XRGB(255, 0, 255));

    // ✅ 공격 애니메이션 44장 미리 로드
    char attackFile[128];
    for (int i = 0; i < 44; ++i) {
        sprintf(attackFile, "./resource/Img/Hero/Hero_attack%03d.png", i);
        if (!m_attackFrames[i].Create(attackFile, true, D3DCOLOR_XRGB(255, 0, 255))) {
            
            return false;
        }
    }
    m_hp = 100;
    m_maxHP = 100;
    m_isDead = false;
    m_isInvincible = false;

    return spriteOk && effectOk;
}

/*  애니메이션 설정 */
void Hero::SetAnim(Anim newAnim, bool forceRestart)
{
    // ★ 죽었으면 DIE 애니 외에는 애니메이션 변경 불가
    if (m_isDead && newAnim != Anim::DIE)
        return;

    ULONGLONG now = GetTickCount64();

    // 공격 애니메이션 최소 유지시간 체크 (단, 죽음 애니는 예외)
    if (m_curAnim == Anim::ATTACK && newAnim != Anim::ATTACK && newAnim != Anim::DIE) {
        if (now - m_attackAnimStartTime < 500)
            return;
    }

    // 공격 애니메이션 시작 처리
    if (newAnim == Anim::ATTACK && (m_curAnim != Anim::ATTACK || forceRestart)) {
        m_attackAnimStartTime = now;
        m_attackDir = m_dir;

        if (m_attackStage == 2)
            m_frame = 6; // 2타 시작 프레임
        else
            m_frame = 0; // 1타 시작 프레임

        m_curAnim = newAnim;
        m_animTime = now;
        return;
    }

    // 중복 설정 방지
    if (m_curAnim == newAnim && !forceRestart)
        return;

    // 애니메이션 상태 변경
    m_curAnim = newAnim;
    m_animTime = now;
    m_frame = 0;

    if (newAnim == Anim::ROLL) {
        m_isRolling = true;
        m_rollDistance = 0.f;
        m_rollEffectFrame = 0;
        m_rollEffectTime = m_animTime;

        m_isInvincible = true;
        m_invincibleEndTime = m_animTime + 500; // 0.5초 무적
    }
    else {
        // ROLL이나 ATTACK이 아닐 때만 무적 해제
        if (m_curAnim != Anim::ATTACK)
            m_isInvincible = false;

        m_isRolling = false;
    }
}

/*  애니메이션 프레임 진행 */
void Hero::advanceFrame()
{
    const AnimInfo& info = animTable[static_cast<int>(m_curAnim)];

    // === 1. DIE 애니메이션: 마지막 프레임에서 정지 ===
    if (m_curAnim == Anim::DIE) {
        if (m_frame < info.totalFrames() - 1) {
            ++m_frame;
            char buf[64];
            sprintf_s(buf, "[Hero] DIE 애니 진행중 - frame: %d\n", m_frame);
            OutputDebugStringA(buf);
        }
        else {
            // 마지막 프레임에 도달했으면 더 이상 증가하지 않음
            OutputDebugStringA("[Hero] DIE 애니 종료\n");
        }
        return;
    }

    // === 2. ATTACK 애니메이션 ===
    if (m_curAnim == Anim::ATTACK) {
        if (m_attackStage == 1) {
            if (m_frame < 5) {  // 1타는 0~5
                ++m_frame;


           
        }
            else {
                // 1타 종료 → 콤보 대기
                m_comboWindowActive = true;
                m_comboWindowStartTime = GetTickCount64();
                SetAnim(Anim::IDLE, true); // 일단 IDLE 전환 (유지 여부는 Update 쪽에서)
            }
        }
        else if (m_attackStage == 2) {
            if (m_frame < 10) { // 2타는 6~10
                ++m_frame;

              
            }
            else {
                m_attackStage = 0;
                m_comboWindowActive = false;
                SetAnim(Anim::IDLE, true);
            }
        }
        return;
    }

    // === 3. ROLL 애니메이션 ===
    if (m_curAnim == Anim::ROLL) {
        if (m_frame + 1 >= info.framesPerRow) {
            m_isRolling = false;
            m_isInvincibleByRoll = false;

            if (key.Pressing(VK_UP) || key.Pressing(VK_DOWN) ||
                key.Pressing(VK_LEFT) || key.Pressing(VK_RIGHT)) {
                SetAnim(Anim::WALK, true);
            }
            else {
                SetAnim(Anim::IDLE, true);
            }
        }
        else {
            ++m_frame;
        }
        return;
    }

    // === 4. 일반 애니메이션 ===
    if (m_frame + 1 >= info.framesPerRow) {
        if (info.loop)
            m_frame = 0;
        else
            m_frame = info.framesPerRow - 1;
    }
    else {
        ++m_frame;
    }
}

/* ===== 매 프레임 업데이트 ===== */
void Hero::Update()
{
    static ULONGLONG lastUpdateTime = 0;
    ULONGLONG now = GetTickCount64();



    // 1) HP 체크 후 즉시 죽음 처리
    if (m_hp <= 0 && !m_isDead)
    {
        m_isDead = true;
        SetAnim(Anim::DIE, false);
        m_frame = 0;
        m_animTime = now;

        // 무적 상태, 공격, 구르기 모두 중단
        m_isInvincible = true;
        m_isInvincibleByHit = true;
        m_isRolling = false;
        m_attackStage = 0;
        m_comboWindowActive = false;

        return; // 죽음 상태로 바로 진입, 이후 업데이트 중단
    }

    if (g_UIManager.IsInventoryOpen()) {
        return; // 인벤토리 열리면 업데이트 중단 (이동/공격/구르기 등 막음)
    }

    if (m_isDead) {
        const AnimInfo& info = animTable[static_cast<int>(m_curAnim)];
        if (m_frame >= info.totalFrames() - 1) {
            // 죽음 애니 끝났으면 업데이트 종료
            return;
        }
        // 애니 프레임 진행 처리
        if (now - m_animTime >= FRAME_MS) {
            advanceFrame();
            m_animTime = now;
        }
        // 무적 유지
        m_isInvincible = true;
        m_isInvincibleByHit = true;
        lastUpdateTime = 0;
        return;
    }
    if (lastUpdateTime == 0)
        lastUpdateTime = now;

    float deltaMs = static_cast<float>(now - lastUpdateTime);
    lastUpdateTime = now;
    float deltaSec = deltaMs / 1000.f;


    // 걷기 사운드 재생 쿨타임 갱신
    if (m_curAnim == Anim::WALK)
    {
        walkSoundTimer -= deltaSec;
        if (walkSoundTimer <= 0.f)
        {
            soundManager.PlaySFX("walk");
            walkSoundTimer = 0.3f; // 0.3초 간격으로 걷기 사운드 재생
        }
    }
    else
    {
        walkSoundTimer = 0.f;

    }

    // 1, 2타는 키 누름 순간에만 콤보 진행, 3타 이상은 키 누름 유지 시 루프
    // ===== 공격 키 입력 처리 =====
    if (!m_isRolling) {
        if (key.Down('J')) {
           
            if (m_curAnim != Anim::ATTACK) {
                m_attackStage = 1;
                m_comboWindowActive = false;
                SetAnim(Anim::ATTACK, true);
                m_lastAttackTime = now;
                m_hasDealtDamageStage1 = false;
                m_hasDealtDamageStage2 = false;
                soundManager.PlaySFX("Sword");
            }
            else if (m_attackStage == 1 && m_comboWindowActive) {
                m_attackStage = 2;
                m_comboWindowActive = false;
                SetAnim(Anim::ATTACK, true);
                m_lastAttackTime = now;
                m_hasDealtDamageStage1 = false;
                m_hasDealtDamageStage2 = false;
                soundManager.PlaySFX("Sword");
            }


        }
    }


    // ===== 콤보 윈도우 및 타임아웃 =====
    if (m_attackStage > 0) {
        const ULONGLONG comboTimeout = 700;
        const ULONGLONG attackMinDuration = 300;

        if (now - m_lastAttackTime > comboTimeout) {
            if (m_curAnim == Anim::ATTACK && now - m_animTime >= attackMinDuration) {
                m_attackStage = 0;
                SetAnim(Anim::IDLE, false);
            }
        }
    }
    // ✅ [추가] 1타 후 콤보 입력 안 들어오면 IDLE로
    if (m_attackStage == 1 && m_comboWindowActive) {
        constexpr ULONGLONG comboWindowDuration = 300;
        if (now - m_comboWindowStartTime > comboWindowDuration) {
            m_comboWindowActive = false;
            m_attackStage = 0;
            SetAnim(Anim::IDLE, true);
        }
    }
    if (m_curAnim == Anim::ATTACK) {
        RECT attackRc = GetAttackHitbox();

        // 1단계 공격 판정
        if (m_attackStage == 1 && m_frame >= 1 && m_frame <= 5 && !m_hasDealtDamageStage1) {
            for (auto& mon : monsters) {
                if (!mon->IsDead()) {
                    RECT monRc = mon->GetCollisionBox();
                    RECT overlap;
                    if (IntersectRect(&overlap, &attackRc, &monRc)) {
                        mon->TakeDamage(50);
                        m_hasDealtDamageStage1 = true;
                        // 데미지 텍스트 출력 (몬스터 위치 위로 30만큼)
                        D3DXVECTOR2 hitPos(mon->GetX(), mon->GetY() - 30);
                        Game::GetInstance()->GetDamageTextManager().AddText(hitPos, 50);
                    }
                }
            }
            // 보스 데미지 판정 추가
            if (boss && !boss->IsDead()) {
                RECT bossRc = boss->GetCollisionBox();
                RECT overlap;
                if (IntersectRect(&overlap, &attackRc, &bossRc)) {
                    boss->TakeDamage(50);
                    m_hasDealtDamageStage1 = true;
                    // 보스 데미지 텍스트 추가
                    D3DXVECTOR2 dmgPos(boss->GetX() -150 , boss->GetY() - 150);
                    Game::GetInstance()->GetDamageTextManager().AddText(dmgPos, 50);
                }
            }
        }
        // 2단계 공격 판정
        else if (m_attackStage == 2 && m_frame >= 6 && m_frame <= 10 && !m_hasDealtDamageStage2) {
            for (auto& mon : monsters) {
                if (!mon->IsDead()) {
                    RECT monRc = mon->GetCollisionBox();
                    RECT overlap;
                    if (IntersectRect(&overlap, &attackRc, &monRc)) {
                        mon->TakeDamage(50);
                        m_hasDealtDamageStage2 = true;
                        // 데미지 텍스트 출력 (몬스터 위치 위로 30만큼)
                        D3DXVECTOR2 hitPos(mon->GetX(), mon->GetY() - 30);
                        Game::GetInstance()->GetDamageTextManager().AddText(hitPos, 50);
                    }
                }
            }
            // 보스 데미지 판정 추가
            if (boss && !boss->IsDead()) {
                RECT bossRc = boss->GetCollisionBox();
                RECT overlap;
                char buffer[256];
                RECT attackRc = GetAttackHitbox();

               

                if (IntersectRect(&overlap, &attackRc, &bossRc)) {
                    OutputDebugStringA("Collision detected! Dealing damage...\n");
                    boss->TakeDamage(50);
                    m_hasDealtDamageStage2 = true;

                    // 보스 데미지 텍스트 추가
                    D3DXVECTOR2 dmgPos(boss->GetX() -150, boss->GetY() - 120);
                    Game::GetInstance()->GetDamageTextManager().AddText(dmgPos, 50);
                }
            }
        }

        // 마지막 프레임 도달 시 플래그 초기화
        if (m_frame == animTable[(int)Anim::ATTACK].totalFrames() - 1) {
            m_hasDealtDamageStage1 = false;
            m_hasDealtDamageStage2 = false;
        }
    }

    // ===== 애니메이션 프레임 갱신 (구르기면 느리게) =====
    ULONGLONG animDelay = FRAME_MS;
    if (m_curAnim == Anim::ROLL) {
        animDelay = 65;  // 구르기 애니메이션 프레임 딜레이를 100ms로 느리게 설정
    }
    else if (m_curAnim == Anim::ATTACK) {
        animDelay = 70;  // 숫자가 클수록 느려짐 (예: 90~120 시도해보자)
    }

    if (now - m_animTime >= animDelay) {
        advanceFrame();
        m_animTime = now;
    }
    // ===== 7. 무적 상태 해제 =====
    if (!m_isDead) {  // ★ 죽었을 경우 무적 해제 안 함
        if (m_isInvincible) {
            m_invincibleTimer += deltaSec;
            if (m_invincibleTimer > 1.0f) {
                m_isInvincible = false;
                m_invincibleTimer = 0.0f;
            }
        }
    }


    if (now > m_invincibleEndTime) {
        m_isInvincible = false;
        m_isInvincibleByHit = false;
    }

    // ===== 구르기 처리 =====
    if (m_isRolling) {
        float moveX = 0.f, moveY = 0.f;

        // 이동 방향 설정
        switch (m_dir) {
        case Direction::DIR_UP:    moveX = m_rollSpeed * 1.0f; break;   // ←
        case Direction::DIR_DOWN:  moveX = -m_rollSpeed * 1.0f; break;  // →
        case Direction::DIR_LEFT:  moveY = -m_rollSpeed * 1.0f; break;  // ↑
        case Direction::DIR_RIGHT: moveY = m_rollSpeed * 1.0f; break;   // ↓
        }
        float tryX = m_x + moveX;
        float tryY = m_y + moveY;

        int offsetL = 15, offsetT = 0, offsetR = 45, offsetB = 50;
        RECT tryRect = {
            static_cast<LONG>(tryX) + offsetL,
            static_cast<LONG>(tryY) + offsetT,
            static_cast<LONG>(tryX) + FRAME_W - offsetR,
            static_cast<LONG>(tryY) + FRAME_H - offsetB
        };

        // 맵 내부 체크
        bool insideMap = true;

        // 왼쪽, 오른쪽 벽
        if (tryRect.left < coll.m_rc.left || tryRect.right > coll.m_rc.right)
            insideMap = false;

        // 위쪽 벽
        if (tryRect.top < coll.m_rc.top) {
            m_y = coll.m_rc.top - offsetT;
            insideMap = false;
        }

        // 아래쪽 벽
        if (tryRect.bottom > coll.m_rc.bottom) {
            m_y = coll.m_rc.bottom - (FRAME_H - offsetB);
            insideMap = false;
        }


        if (insideMap) {
            // 맵 내부면 이동 허용
            m_x = tryX;
            m_y = tryY;

            // 이동 거리 누적
            m_rollDistance += sqrt(moveX * moveX + moveY * moveY);
        }
        else {
            // 맵 밖이면 밀리는 모션만 주고 위치 유지
            const float knockback = 2.f;

            switch (m_dir)
            {
            case Direction::DIR_UP:    m_x -= knockback; break; // ←
            case Direction::DIR_DOWN:  m_x += knockback; break; // →
            case Direction::DIR_LEFT:  m_y -= knockback; break; // ↑
            case Direction::DIR_RIGHT: m_y += knockback; break; // ↓
            }
            // 구르기 종료하지 않고 모션 유지
        }

        // 구르기 이펙트 위치 계산
        float effectX = m_x;
        float effectY = m_y;
        const float effectOffset = 20.f;


        switch (m_dir) {
        case Direction::DIR_UP:
            effectY = m_y + effectOffset;
            effectX = m_x + FRAME_W / 2;
            break;
        case Direction::DIR_DOWN:
            effectY = m_y - effectOffset;
            effectX = m_x + FRAME_W / 2;
            break;
        case Direction::DIR_LEFT:
            effectX = m_x + FRAME_W + effectOffset;
            effectY = m_y + FRAME_H / 2;
            break;
        case Direction::DIR_RIGHT:
            effectX = m_x - effectOffset;
            effectY = m_y + FRAME_H / 2;
            break;
        }

        // ===== 7. 이펙트 프레임 갱신 =====
        if (now - m_rollEffectTime >= ROLL_EFFECT_FRAME_DELAY) {
            ++m_rollEffectFrame;

            if (m_rollEffectFrame >= ROLL_EFFECT_FRAME_COUNT)
                m_rollEffectFrame = 0;
            m_rollEffectTime = now;
        }

        // ===== 8. 이펙트 그리기 =====
        DrawRollEffect();
    }
    UpdateCollisionBox();

    // 디버그 출력
    /*char buf[128];
    sprintf(buf, "Hero Pos: (%.1f, %.1f) | CollisionBox: (L:%ld, T:%ld, R:%ld, B:%ld)\n",
        m_x, m_y, m_rc.left, m_rc.top, m_rc.right, m_rc.bottom);
    OutputDebugStringA(buf);*/
    }

/* ===== 방향에 따라 IDLE 전환 방향 지정 ===== */ // 대기모션 함수 구현해놓은거 대기모션 바꾸고싶으면 여기 바꾸기
Hero::Direction Hero::GetIdleDirectionFromMove(Hero::Direction moveDir)
{
    switch (moveDir)
    {
    case Direction::DIR_RIGHT: return Direction::DIR_DOWN;
    case Direction::DIR_UP:    return Direction::DIR_LEFT;
    case Direction::DIR_LEFT:  return Direction::DIR_UP;
    case Direction::DIR_DOWN:  return Direction::DIR_RIGHT;
    default: return moveDir;
    }
}

/* ===== 스프라이트 시트 내 프레임 계산 ===== */
void Hero::calcSrcRect(RECT& rc)
{
    // ⭐️ 공격은 별도 이미지 사용
    if (m_curAnim == Anim::ATTACK)
    {
        int baseIndex = 0;
        switch (m_dir) {
        case Direction::DIR_DOWN:     baseIndex = 33; break;
        case Direction::DIR_UP:       baseIndex = 22; break;
        case Direction::DIR_RIGHT:    baseIndex = 0; break;
        case Direction::DIR_LEFT:     baseIndex = 11; break;
        }

        int attackIndex = baseIndex + m_frame;

        // 공격 이미지 파일명
        char filename[64];
        sprintf(filename, "./resource/Img/Hero/Hero_attack%03d.png", attackIndex);

        // 스프라이트 이미지 교체 (한 프레임만 사용하므로 전체 영역 사용)
       /* m_sprite.Create(filename, true, D3DCOLOR_XRGB(255, 0, 255));*/

        rc.left = 0;
        rc.top = 0;
        rc.right = 120;
        rc.bottom = 120;
        return; // ⭐️ 아래 코드 실행 안 하도록 리턴
    }

    // ⭐️ 나머지 애니메이션 (WALK, IDLE, ROLL 등)
    const AnimInfo& info = animTable[static_cast<int>(m_curAnim)];
    int col = m_frame;
    int row;

    if (m_curAnim == Anim::IDLE)
    {
        Direction idleDir = GetIdleDirectionFromMove(m_dir);
        row = info.rowStart + static_cast<int>(idleDir);
    }
    else if (m_curAnim == Anim::WALK)
    {
        row = info.rowStart + static_cast<int>(m_dir);

    }
    else if (m_curAnim == Anim::ROLL)
    {
        switch (m_dir)
        {
        case Direction::DIR_UP: row = info.rowStart + 0; break;
        case Direction::DIR_DOWN: row = info.rowStart + 1; break;
        case Direction::DIR_LEFT: row = info.rowStart + 2; break;
        case Direction::DIR_RIGHT: row = info.rowStart + 3; break;
        }
    }

    else if (m_curAnim == Anim::DIE)
    {
        row = info.rowStart;  // 항상 고정 한 줄
    }

    else
    {
        row = info.rowStart + static_cast<int>(m_dir);
    }

    rc.left = col * FRAME_W;
    rc.top = row * FRAME_H;
    rc.right = rc.left + FRAME_W;
    rc.bottom = rc.top + FRAME_H;
}


/* ===== 화면 출력 ===== */
void Hero::Draw()
{
    RECT src;
    calcSrcRect(src);

    ULONGLONG now = GetTickCount64();
    bool recentlyHit = (now - m_lastHitTime <= DAMAGE_COLOR_DURATION);

    D3DCOLOR drawColor;


    if (IsDead()) {
        drawColor = D3DCOLOR_XRGB(255, 255, 255); // 죽었을 땐 그냥 하얀색
    }
    else if (m_isInvincibleByRoll) {
        drawColor = D3DCOLOR_ARGB(160, 100, 200, 255); // 💙 구르기 무적일 땐 파란 반투명
    }
    else if (m_isInvincibleByHit || recentlyHit) {
        drawColor = D3DCOLOR_ARGB(160, 255, 100, 100); // ❤️ 피격 무적일 땐 빨간 반투명
    }
    else {
        drawColor = D3DCOLOR_XRGB(255, 255, 255); // 평상시 흰색
    }

    // D3DCOLOR → 개별 값 추출
    int a = (drawColor >> 24) & 0xFF;
    int r = (drawColor >> 16) & 0xFF;
    int g = (drawColor >> 8) & 0xFF;
    int b = (drawColor) & 0xFF;

    if (m_curAnim == Anim::ATTACK) {
        int baseIndex = 0;
        switch (m_attackDir) {
        case Direction::DIR_DOWN:  baseIndex = 33; break;
        case Direction::DIR_UP:    baseIndex = 22; break;
        case Direction::DIR_RIGHT: baseIndex = 0; break;
        case Direction::DIR_LEFT:  baseIndex = 11; break;
        }

        // m_frame은 상대값 0~10 → 최종 인덱스 = base + m_frame
        int index = baseIndex + m_frame;

        if (index >= 0 && index < 44) {
            m_attackFrames[index].RenderDraw(m_x, m_y, 0, 0, 120, 120, 0.f, 1.f, 1.f);
        }
        if (g_showColliderBox)
        coll.BoxSow(m_rc, 0, -5, 0xffff0000);
        return;
    }

    // ===== 구르기 이펙트 =====
    DrawRollEffect();

    // ⭐ DIE 애니메이션 별도 처리
    if (m_curAnim == Anim::DIE) {
        int row = animTable[(int)Anim::DIE].rowStart;
        int col = m_frame % animTable[(int)Anim::DIE].framesPerRow;

        int x = col * FRAME_WIDTH;
        int y = row * FRAME_HEIGHT;

        RECT dieSrc = { x, y, x + FRAME_WIDTH, y + FRAME_HEIGHT };

        m_sprite.SetColor(r, g, b, a);
        m_sprite.RenderDraw(m_x, m_y,
            static_cast<float>(dieSrc.left), static_cast<float>(dieSrc.top),
            static_cast<float>(dieSrc.right), static_cast<float>(dieSrc.bottom),
            0.f, 1.f, 1.f);
        if (g_showColliderBox)
        coll.BoxSow(m_rc, 0, -5, 0xffff0000);
        return;
    }


    m_sprite.SetColor(r, g, b, a); 
    m_sprite.RenderDraw(
        m_x, m_y,
        static_cast<float>(src.left), static_cast<float>(src.top),
        static_cast<float>(src.right), static_cast<float>(src.bottom),
        0.0f, 1.0f, 1.0f);
    if (g_showColliderBox)
    coll.BoxSow(m_rc, 0, -5, 0xffff0000);

}


/* ===== 외부에서 방향 설정 ===== */
void Hero::SetDirection(Direction dir)
{
    if (IsDead() || IsAttacking() ) return; // 죽으면 방향 안 바뀜
    m_dir = dir;
}

/* ===== 외부에서 ROLL 시작 시 호출 ===== */
void Hero::StartRoll()
{
    if (IsAttacking() || m_isRolling)  // ✅ ROLL 중복 방지
        return;  
    soundManager.PlaySFX("roll");


    SetAnim(Anim::ROLL, true);
    m_isRolling = true;
    m_isInvincibleByRoll = true;
    m_rollDistance = 0.f;         // 구르기 거리 초기화
    m_rollEffectFrame = 0;
    m_rollEffectTime = GetTickCount64();

}

/* ===== 무적 여부 확인 (외부에서 사용 가능) ===== */
bool Hero::IsInvincible() const {
    // 죽었으면 무조건 무적(true) 상태로 처리해서 데미지 안 받게 함
    return m_isInvincible || m_hp <= 0 || m_isDead;

}

void Hero::DrawRollEffect()
{
    if (!m_isRolling) return;

    const int cutLeft = 10;  // 필요하면 조정

    float frameW = m_rollEffect.imagesinfo.Width / ROLL_EFFECT_FRAME_COUNT;
    float frameH = m_rollEffect.imagesinfo.Height;

    LONG left = static_cast<LONG>(frameW * m_rollEffectFrame) + cutLeft;
    LONG right = static_cast<LONG>(frameW * (m_rollEffectFrame + 1));
    if (right - left < 1) right = left + 1;

    RECT srcRect = { left, 0, right, static_cast<LONG>(frameH) };

    float effectX = m_x + FRAME_W / 2.f + cutLeft;
    float effectY = m_y + FRAME_H / 2.f;

    float rotation = 0.f;

    switch (m_dir) {
    case Direction::DIR_UP:
        effectY -= 10.f;
        rotation = D3DXToRadian(0.f);
        break;
    case Direction::DIR_DOWN:
        effectY += 10.f;
        rotation = D3DXToRadian(180.f);
        break;
    case Direction::DIR_LEFT:
        effectX -= 10.f;
        rotation = D3DXToRadian(270.f);
        break;
    case Direction::DIR_RIGHT:
        effectX += 10.f;
        rotation = D3DXToRadian(90.f);
        break;
    }

    m_rollEffect.SetColor(255, 255, 255, 100);

    m_rollEffect.RenderDraw(effectX, effectY,
        static_cast<float>(srcRect.left),
        static_cast<float>(srcRect.top),
        static_cast<float>(srcRect.right - srcRect.left),
        static_cast<float>(srcRect.bottom - srcRect.top),
        rotation,
        1.f, 1.f);

    m_rollEffect.SetColor(255, 255, 255, 255);
}

bool Hero::IsAttacking() const {
    return m_attackStage > 0;
}
RECT Hero::GetAttackHitbox() const {

    RECT rc = m_rc;
    int attackRangeLeftRight = 120;  // 좌우 공격 범위 (좀 넓게)
    int attackRangeUp = 40;          // 위 공격 범위 (기존대로)
    int attackRangeDown = 30;        // 아래 공격 범위 (좀 줄임)

    switch (m_dir) {
    case Direction::DIR_LEFT:
        rc.left -= attackRangeLeftRight;
        rc.right = m_rc.left;
        rc.top += 10;    // Y축 위쪽 자름 (위로 10만큼)
        rc.bottom -= 10; // Y축 아래쪽 자름 (아래로 10만큼)
        break;
    case Direction::DIR_RIGHT:
        rc.left = m_rc.right;
        rc.right += attackRangeLeftRight;
        rc.top += 10;
        rc.bottom -= 10;

        break;
    case Direction::DIR_UP:
        rc.top -= attackRangeUp;
        rc.bottom = m_rc.top;
        rc.left += 10;   // X축 좌우 좁힘
        rc.right -= 10;
        break;
    case Direction::DIR_DOWN:
        rc.top = m_rc.bottom;
        rc.bottom += attackRangeDown;
        rc.left += 10;
        rc.right -= 10;
        break;
    }
    return rc;
}

void Hero::TakeDamage(int damage) {
    if (m_isInvincible || IsDead()) return;  // 무적 상태거나 이미 죽었으면 무시

 

    m_hp -= damage;

    // 강제로 출력
    char buf[128];
    sprintf_s(buf, "Hero::TakeDamage 호출 - m_hp: %d, m_maxHP: %d\n", m_hp, m_maxHP);
    OutputDebugStringA(buf);

    // UI 갱신
    g_UIManager.SetHP(m_hp, m_maxHP);

    // 디버깅 출력
    OutputDebugStringA("[Hero] 데미지 받음! 남은 HP: ");
    char buffer[32];
    sprintf_s(buffer, "%d\n", m_hp);
    OutputDebugStringA(buffer);

    if (m_hp <= 0) {
        m_hp = 0;
        m_isDead = true;
        OutputDebugStringA("[Hero] 사망 처리 완료\n");

        SetAnim(Anim::DIE, true);
        return;  // 🔥 죽었으면 더 이상 처리 안 함
    }
    

    // 살아있을 경우 무적 처리
    m_isInvincible = true;
    m_isInvincibleByHit = true;
    m_invincibleEndTime = GetTickCount64() + 1000;
    m_lastHitTime = GetTickCount64();
}





RECT Hero::GetCollisionBox() const {
    return m_rc;
}

bool Hero::IsDead() const {
    return m_isDead || m_hp <= 0;
}


void Hero::Heal(int amount)
{
    m_hp += amount;
    if (m_hp > m_maxHP)
        m_hp = m_maxHP;
    soundManager.PlaySFX("heal");  // 여기서 회복 사운드 재생

    OutputDebugStringA("[Hero] 체력 회복됨\n");
}

void Hero::UpdateCollisionBox()
{
    int offsetL = 15, offsetT = 10, offsetR = 45, offsetB = 35;
    m_rc.left = static_cast<LONG>(m_x) + offsetL;
    m_rc.top = static_cast<LONG>(m_y) + offsetT;
    m_rc.right = static_cast<LONG>(m_x) + FRAME_W - offsetR;
    m_rc.bottom = static_cast<LONG>(m_y) + FRAME_H - offsetB;
}