#define WIN32_LEAN_AND_MEAN
#include "Global.h"
#include "BossSkeleton.h"
#include "Hero.h"
#include "GlobalSprite.h"
#include "Game.h"
#include "SoundManager.h"
extern SoundManager soundManager;

extern Game game;

#define BOSS_SCALE 1.5f
extern LPD3DXSPRITE g_sprite;

void DebugPrintRect(const char* name, const RECT& rc)
{
   

}
// ==============================
// 생성자
// ==============================
BossSkeleton::BossSkeleton(float startX, float startY, LPD3DXSPRITE spr)
{
    // 위치 초기화
    m_x = startX;
    m_y = startY;

    // 기본 방향은 아래쪽
    m_direction = BossDirection::Down;

    // 기본 상태는 대기
    m_state = BossState::Idle;
    m_stateTimer = 0.f;

    // 체력 설정
    m_bossHp = 500;
    m_bossMaxHp = 500;

    // 공격 초기 상태
    m_currentAttackPattern = BossAttackPattern::None;
    m_isAttacking = false;

    // 감정 상태
    m_hasEnteredPhase2 = false;

    // 애니메이션 및 충돌 프레임 초기화
    m_moveFrame = 0;
    m_moveFrameTimer = 0.f;
    m_jumpYOffset = 0.f;

   

    // 나중에 구현될 프레임 로딩 함수 호출
    LoadBossMoveFrames();
    LoadBossHammerFrames();
    LoadBossSwordFrames();
}

// ==============================
// 소멸자
// ==============================
BossSkeleton::~BossSkeleton()
{
    // 이동 프레임 텍스처 해제
    for (auto& tex : moveFramesDown) if (tex) tex->Release();
    for (auto& tex : moveFramesLeft) if (tex) tex->Release();
    for (auto& tex : moveFramesRight) if (tex) tex->Release();
    for (auto& tex : moveFramesUp) if (tex) tex->Release();

    moveFramesDown.clear();
    moveFramesLeft.clear();
    moveFramesRight.clear();
    moveFramesUp.clear();

    // 해머 공격 프레임 텍스처 해제
    for (auto& tex : hammerFramesUp) if (tex) tex->Release();
    for (auto& tex : hammerFramesDown) if (tex) tex->Release();
    for (auto& tex : hammerFramesLeft) if (tex) tex->Release();
    for (auto& tex : hammerFramesRight) if (tex) tex->Release();
    for (auto& tex : hammerFramesAll) if (tex) tex->Release();

    hammerFramesUp.clear();
    hammerFramesDown.clear();
    hammerFramesLeft.clear();
    hammerFramesRight.clear();
    hammerFramesAll.clear();

    // 검 공격 프레임 텍스처 해제
    for (auto& tex : swordFramesUp) if (tex) tex->Release();
    for (auto& tex : swordFramesDown) if (tex) tex->Release();
    for (auto& tex : swordFramesLeft) if (tex) tex->Release();
    for (auto& tex : swordFramesRight) if (tex) tex->Release();
    for (auto& tex : swordFramesAll) if (tex) tex->Release();

    swordFramesUp.clear();
    swordFramesDown.clear();
    swordFramesLeft.clear();
    swordFramesRight.clear();
    swordFramesAll.clear();
}

void BossSkeleton::LoadBossMoveFrames()
{
    // 아래쪽 프레임: bossSkeletonMove000~013
    for (int i = 0; i <= 13; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/move/bossSkeletonMove%03d.png", i);

        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
        {
            moveFramesDown.push_back(frameTex);
        }
    }

    // 왼쪽 프레임: bossSkeletonMove014~027
    for (int i = 14; i <= 27; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/move/bossSkeletonMove%03d.png", i);

        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
        {
            moveFramesLeft.push_back(frameTex);
        }
    }

    // 오른쪽 프레임: bossSkeletonMove028~041
    for (int i = 28; i <= 41; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/move/bossSkeletonMove%03d.png", i);

        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
        {
            moveFramesRight.push_back(frameTex);
        }
    }

    // 위쪽 프레임: bossSkeletonMove042~055
    for (int i = 42; i <= 55; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/move/bossSkeletonMove%03d.png", i);

        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
        {
            moveFramesUp.push_back(frameTex);
        }
    }
}
void BossSkeleton::LoadBossHammerFrames()
{
    // 위쪽: bossSkeletonHammer000~019
    for (int i = 0; i <= 19; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/hammer/bossSkeletonHammer%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            hammerFramesUp.push_back(frameTex);
    }

    // 오른쪽: bossSkeletonHammer020~039
    for (int i = 20; i <= 39; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/hammer/bossSkeletonHammer%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            hammerFramesRight.push_back(frameTex);
    }

    // 왼쪽: bossSkeletonHammer040~059
    for (int i = 40; i <= 59; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/hammer/bossSkeletonHammer%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            hammerFramesLeft.push_back(frameTex);
    }

    // 아래쪽: bossSkeletonHammer060~079
    for (int i = 60; i <= 79; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/hammer/bossSkeletonHammer%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            hammerFramesDown.push_back(frameTex);
    }
    hammerFramesAll.clear();
    hammerFramesAll.insert(hammerFramesAll.end(), hammerFramesUp.begin(), hammerFramesUp.end());
    hammerFramesAll.insert(hammerFramesAll.end(), hammerFramesRight.begin(), hammerFramesRight.end());
    hammerFramesAll.insert(hammerFramesAll.end(), hammerFramesLeft.begin(), hammerFramesLeft.end());
    hammerFramesAll.insert(hammerFramesAll.end(), hammerFramesDown.begin(), hammerFramesDown.end());
}

void BossSkeleton::LoadBossSwordFrames()
{
    // 오른쪽: bossSkeletonSword000~021
    for (int i = 0; i <= 21; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/sword/bossSkeletonSword%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            swordFramesRight.push_back(frameTex);
    }

    // 위쪽: bossSkeletonSword022~043
    for (int i = 22; i <= 43; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/sword/bossSkeletonSword%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            swordFramesUp.push_back(frameTex);
    }

    // 아래쪽: bossSkeletonSword044~065
    for (int i = 44; i <= 65; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/sword/bossSkeletonSword%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            swordFramesDown.push_back(frameTex);
    }

    // 왼쪽: bossSkeletonSword066~087
    for (int i = 66; i <= 87; ++i)
    {
        char fileName[128];
        sprintf(fileName, "./resource/Img/boss/sword/bossSkeletonSword%03d.png", i);
        LPDIRECT3DTEXTURE9 frameTex = nullptr;
        HRESULT hr = D3DXCreateTextureFromFile(g_pd3dDevice, fileName, &frameTex);
        if (SUCCEEDED(hr) && frameTex != nullptr)
            swordFramesLeft.push_back(frameTex);
    }
    swordFramesAll.clear();
    swordFramesAll.insert(swordFramesAll.end(), swordFramesRight.begin(), swordFramesRight.end());
    swordFramesAll.insert(swordFramesAll.end(), swordFramesUp.begin(), swordFramesUp.end());
    swordFramesAll.insert(swordFramesAll.end(), swordFramesDown.begin(), swordFramesDown.end());
    swordFramesAll.insert(swordFramesAll.end(), swordFramesLeft.begin(), swordFramesLeft.end());
}


void BossSkeleton::Update(float deltaTime)
{
    if (!m_hero) return;

    // 상태 지속 시간 누적
    m_stateTimer += deltaTime;

    // 걷기 애니메이션 프레임 타이머 업데이트
    if (m_state == BossState::Idle || m_state == BossState::Chase)
    {
        m_moveFrameTimer += deltaTime;
        if (m_moveFrameTimer >= MOVE_FRAME_DURATION)  // 예: 0.1f초 간격
        {
            m_moveFrame = (m_moveFrame + 1) % moveFramesUp.size(); // 프레임 넘버 증가, 배열 크기에 맞게
            m_moveFrameTimer = 0.f;
            // 걷기 소리 재생 (프레임 변경 시마다)
            if (m_stepSoundTimer <= 0.f)  // 쿨타임 체크용 변수 (멤버 변수로 선언)
            {
                soundManager.PlaySFX("skeletonStep1");
                m_stepSoundTimer = 0.02f;  // 0.4초 쿨타임 설정
            }
            // 쿨타임 감소
            if (m_stepSoundTimer > 0.f)
            {
                m_stepSoundTimer -= deltaTime;
            }
        }
    }
    else
    {
        // 걷기 상태가 아닐 때 프레임 초기화 (원하면)
        m_moveFrame = 0;
        m_moveFrameTimer = 0.f;
    }

    // 상태별 처리 (FSM)
    switch (m_state)
    {
    case BossState::Idle:
        if (m_hero && m_stateTimer > 0.05f)
        {
            ChangeState(BossState::Chase);
        }
        break;

    case BossState::Chase:
    {
        float dx = m_hero->GetX() - m_x;
        float dy = m_hero->GetY() - m_y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 0.1f)
        {
            float moveX = m_moveSpeed * deltaTime * dx / dist;
            float moveY = m_moveSpeed * deltaTime * dy / dist;

            float tryX = m_x + moveX;
            float tryY = m_y + moveY;

            if (CanMoveTo(tryX, tryY))
            {
                m_x = tryX;
                m_y = tryY;
            }
            else
            {
                const float knockback = 2.f;
                switch (m_direction)
                {
                case BossDirection::Up:    m_y += knockback; break;
                case BossDirection::Down:  m_y -= knockback; break;
                case BossDirection::Left:  m_x += knockback; break;
                case BossDirection::Right: m_x -= knockback; break;
                }
            }

            const float THRESHOLD = 0.3f;

            if (fabs(dx) < THRESHOLD) {
                m_direction = (dy > 0) ? BossDirection::Down : BossDirection::Up;
            }
            else if (fabs(dy) < THRESHOLD) {
                m_direction = (dx > 0) ? BossDirection::Right : BossDirection::Left;
            }
            else {
                if (fabs(dx) > fabs(dy))
                    m_direction = (dx > 0) ? BossDirection::Right : BossDirection::Left;
                else
                    m_direction = (dy > 0) ? BossDirection::Down : BossDirection::Up;
            }
        }

        if (dist <= m_attackRange)
        {
            ChangeState(BossState::Attack);
        }
    }
    break;

    case BossState::Attack:
        UpdateAttack(deltaTime);
        break;

   /* case BossState::WaitAfterAttack:
        if (m_stateTimer >= WAIT_AFTER_ATTACK_DURATION)
        {
            ChangeState(BossState::Chase);
        }
        break;*/

    //case BossState::Hit:
    //{
    //    if (m_stateTimer >= 0.1f)
    //    {
    //        ChangeState(BossState::Chase);
    //    }
    //    // Hit 상태에서도 공격은 별도 로직으로 처리해야 함 (필요 시)
    //}
    //break;

    case BossState::Dead:
        // 사망 처리
        break;

    default:
        break;
    }

    // ✅ Hit 이펙트는 공통 처리 (어느 상태에서든 가능)
    if (isHitEffectActive && m_stateTimer >= 0.2f)
    {
        isHitEffectActive = false;
    }

    // ✅ 죽지 않았으면 콜라이더 갱신
    if (m_state != BossState::Dead)
    {
        UpdateCollisionBoxes();
        UpdateAttackHitbox();
    }
    else
    {
        // 죽은 상태에서는 박스를 완전히 제거 (0으로 초기화)
        SetRect(&m_rc, 0, 0, 0, 0);
        SetRect(&m_attackHitbox, 0, 0, 0, 0);
    }
}

void BossSkeleton::ChangeState(BossState newState)
{
    if (m_state == newState)
        return;

    m_state = newState;
    m_stateTimer = 0.f;

    switch (newState)
    {
    case BossState::Idle:
        m_isAttacking = false;
        break;

    case BossState::Chase:
        m_isAttacking = false;
        break;

    case BossState::Attack:
        m_isAttacking = true;

        // 공격 패턴 랜덤 선택 예시 (해머 또는 슬래시)
        if (rand() % 2 == 0)  // 50% 확률
            m_currentAttackPattern = BossAttackPattern::Hammer;
        else
            m_currentAttackPattern = BossAttackPattern::Slash;

        attackAnimTimer = 0.f;
        attackAnimFrame = 0;
        m_hasDealtDamageThisAttack = false;
        break;

    //case BossState::WaitAfterAttack:
    //    m_isAttacking = false;
    //    m_hasDealtDamageThisAttack = false; // 공격 끝난 후 플래그 초기화
    //    break;

   

    case BossState::Dead:
        // 사망 상태 초기화
        break;

    default:
        break;
    }
}

void BossSkeleton::Draw()
{
    // 위치 보정 Y 오프셋(점프 등) 적용
    float drawX = m_x;
    float drawY = m_y + m_jumpYOffset;

    LPDIRECT3DTEXTURE9 currentFrame = nullptr;

    switch (m_state)
    {
   /* case BossState::Intro:*/
    case BossState::Idle:
    case BossState::Chase:
   /* case BossState::WaitAfterAttack:*/
    //case BossState::Hit:   // Hit 상태도 이동 애니메이션 그리기
        switch (m_direction)
        {
        case BossDirection::Up:
            if (!moveFramesUp.empty())
                currentFrame = moveFramesUp[m_moveFrame % moveFramesUp.size()];
            break;
        case BossDirection::Down:
            if (!moveFramesDown.empty())
                currentFrame = moveFramesDown[m_moveFrame % moveFramesDown.size()];
            break;
        case BossDirection::Left:
            if (!moveFramesLeft.empty())
                currentFrame = moveFramesLeft[m_moveFrame % moveFramesLeft.size()];
            break;
        case BossDirection::Right:
            if (!moveFramesRight.empty())
                currentFrame = moveFramesRight[m_moveFrame % moveFramesRight.size()];
            break;
        }
        break;

    case BossState::Attack:
        if (m_currentAttackPattern == BossAttackPattern::None) {
            // 공격 패턴 없으면 이동 애니메이션 그리기
            switch (m_direction)
            {
            case BossDirection::Up:
                if (!moveFramesUp.empty())
                    currentFrame = moveFramesUp[attackAnimFrame % moveFramesUp.size()];
                break;
            case BossDirection::Down:
                if (!moveFramesDown.empty())
                    currentFrame = moveFramesDown[attackAnimFrame % moveFramesDown.size()];
                break;
            case BossDirection::Left:
                if (!moveFramesLeft.empty())
                    currentFrame = moveFramesLeft[attackAnimFrame % moveFramesLeft.size()];
                break;
            case BossDirection::Right:
                if (!moveFramesRight.empty())
                    currentFrame = moveFramesRight[attackAnimFrame % moveFramesRight.size()];
                break;
            }
        }
        else
        {
            switch (m_currentAttackPattern)
            {
            case BossAttackPattern::Slash:
                switch (m_direction)
                {
                case BossDirection::Up:
                    if (!swordFramesUp.empty())
                        currentFrame = swordFramesUp[attackAnimFrame % swordFramesUp.size()];
                    break;
                case BossDirection::Down:
                    if (!swordFramesDown.empty())
                        currentFrame = swordFramesDown[attackAnimFrame % swordFramesDown.size()];
                    break;
                case BossDirection::Left:
                    if (!swordFramesLeft.empty())
                        currentFrame = swordFramesLeft[attackAnimFrame % swordFramesLeft.size()];
                    break;
                case BossDirection::Right:
                    if (!swordFramesRight.empty())
                        currentFrame = swordFramesRight[attackAnimFrame % swordFramesRight.size()];
                    break;
                }
                break;
            case BossAttackPattern::Hammer:
                switch (m_direction)
                {
                case BossDirection::Up:
                    if (!hammerFramesUp.empty())
                        currentFrame = hammerFramesUp[attackAnimFrame % hammerFramesUp.size()];
                    break;
                case BossDirection::Down:
                    if (!hammerFramesDown.empty())
                        currentFrame = hammerFramesDown[attackAnimFrame % hammerFramesDown.size()];
                    break;
                case BossDirection::Left:
                    if (!hammerFramesLeft.empty())
                        currentFrame = hammerFramesLeft[attackAnimFrame % hammerFramesLeft.size()];
                    break;
                case BossDirection::Right:
                    if (!hammerFramesRight.empty())
                        currentFrame = hammerFramesRight[attackAnimFrame % hammerFramesRight.size()];
                    break;
                }
                break;

            default:
                break;
            }
        }
        break;

    case BossState::Dead:
        // 사망 애니메이션 그리기 예시 (필요 시 구현)
        break;

    default:
        break;
    }

    // 기본 흰색
    D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255);

    // ✅ isHitEffectActive 사용: 상태와 무관하게 깜빡임 표시
    if (isHitEffectActive)
    {
        const float blinkInterval = 0.1f;
        int blinkPhase = static_cast<int>(m_stateTimer / blinkInterval) % 2;
        if (blinkPhase == 0)
        {
            color = D3DCOLOR_XRGB(255, 100, 100);  // 붉게 깜빡임
        }
    }

    if (currentFrame)
    {
        RECT srcRect = { 0, 0, 265, 265 };
        float scale = 1.5f;

        D3DXMATRIX matScale, matTrans, matWorld;
        D3DXMatrixScaling(&matScale, scale, scale, 1.0f);
        D3DXMatrixTranslation(&matTrans, drawX, drawY, 0);
        matWorld = matScale * matTrans;

        g_sprite->SetTransform(&matWorld);

        D3DXVECTOR3 center(265 / 2.0f, 265 / 2.0f, 0);
        D3DXVECTOR3 position(0, 0, 0);

        g_sprite->Begin(D3DXSPRITE_ALPHABLEND);
        g_sprite->Draw(currentFrame, &srcRect, &center, &position, color);
        g_sprite->End();

        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity(&matIdentity);
        g_sprite->SetTransform(&matIdentity);
    }

    if (g_showColliderBox && m_state != BossState::Dead)
    {
        coll.BoxSow(m_bossRc, 0, 0, 0xffff0000);      // 본체 충돌 박스 (빨강)
        coll.BoxSow(m_attackHitbox, 0, 0, 0xff00ff00); // 공격 히트박스 (연두)
    }
}

void BossSkeleton::UpdateAttack(float deltaTime)
{

    attackAnimTimer += deltaTime;
    const float frameDuration = 0.015f;

    int animStartIndex = 0;
    int animEndIndex = 0;
    const std::vector<LPDIRECT3DTEXTURE9>* animFrames = nullptr;

    // 공격 패턴 및 방향에 따라 애니메이션 범위 설정
    if (m_currentAttackPattern == BossAttackPattern::Hammer) {
        animFrames = &hammerFramesAll;
        switch (m_direction) {
        case BossDirection::Up:    animStartIndex = 0;  animEndIndex = 19; break;
        case BossDirection::Right: animStartIndex = 20; animEndIndex = 39; break;
        case BossDirection::Left:  animStartIndex = 40; animEndIndex = 59; break;
        case BossDirection::Down:  animStartIndex = 60; animEndIndex = 79; break;
        }
    }
    else if (m_currentAttackPattern == BossAttackPattern::Slash) {
        animFrames = &swordFramesAll;
        switch (m_direction) {
        case BossDirection::Right: animStartIndex = 0;  animEndIndex = 21; break;
        case BossDirection::Up:    animStartIndex = 22; animEndIndex = 43; break;
        case BossDirection::Down:  animStartIndex = 44; animEndIndex = 65; break;
        case BossDirection::Left:  animStartIndex = 66; animEndIndex = 87; break;
        }
    }

    // 공격 히트박스 항상 갱신
    UpdateAttackHitbox();

    // 데미지 처리 및 공격 소리 재생 (1 프레임만)
    const int damageFrame = 7;
    if (attackAnimFrame == damageFrame && !m_hasDealtDamageThisAttack && m_hero && !m_hero->m_isDead)
    {
        // 공격 애니메이션 프레임에서 소리 재생 (한 번만)
        if (m_currentAttackPattern == BossAttackPattern::Hammer) {
            soundManager.PlaySFX("skeletonAttackHammer");
        }
        else if (m_currentAttackPattern == BossAttackPattern::Slash) {
            soundManager.PlaySFX("skeletonAttackSword");
        }

        RECT heroBox = m_hero->GetCollisionBox();
        RECT overlap;
        if (IntersectRect(&overlap, &m_attackHitbox, &heroBox))
        {
            int damage = (m_currentAttackPattern == BossAttackPattern::Hammer)
                ? HAMMER_DAMAGE_AMOUNT
                : SLASH_DAMAGE_AMOUNT;

            m_hero->TakeDamage(damage);

            // 데미지 텍스트 출력
            D3DXVECTOR2 dmgPos(m_hero->GetX() + 30, m_hero->GetY() - 30);
            Game::GetInstance()->GetDamageTextManager().AddText(dmgPos, damage);
        }

        m_hasDealtDamageThisAttack = true;  // 플래그는 여기서 반드시 true로 변경
    }

    // 애니메이션 프레임 갱신.
    if (attackAnimTimer >= frameDuration)
    {
        attackAnimTimer = 0.0f;
        attackAnimFrame += 1;

        int maxFrames = animEndIndex - animStartIndex + 1;
        if (attackAnimFrame >= maxFrames)
        {
            ChangeState(BossState::Chase);
            return;
        }
    }

    // 현재 프레임 텍스처 (Draw에서 활용)
    int currentFrameInRange = attackAnimFrame;
    if (animFrames && currentFrameInRange < (animEndIndex - animStartIndex + 1))
    {
        LPDIRECT3DTEXTURE9 currentTexture = (*animFrames)[animStartIndex + currentFrameInRange];
        // TODO: Draw에서 currentTexture 사용
    }
}


void BossSkeleton::UpdateCollisionBoxes()
{
    // 스프라이트 원본 크기 (이미지 1프레임 크기)
    const int boxWidth = 100;
    const int boxHeight = 120;

    const int offsetX = -80;  // 왼쪽으로 30픽셀 이동
    const int offsetY = -100;  // 위쪽으로 40픽셀 이동

    m_bossRc.left = static_cast<LONG>(m_x + offsetX);
    m_bossRc.top = static_cast<LONG>(m_y + offsetY);
    m_bossRc.right = m_bossRc.left + boxWidth;
    m_bossRc.bottom = m_bossRc.top + boxHeight;

 
}


RECT BossSkeleton::GetCollisionBox() const
{
    return m_bossRc;
}

RECT BossSkeleton::GetAttackRangeBox() const
{
    return m_bossAttackRc;
}
void BossSkeleton::TakeDamage(int damage)
{
    if (m_state == BossState::Dead) return;
    OutputDebugStringA("[Boss] TakeDamage 호출\n");

    m_bossHp -= damage;
    if (m_bossHp <= 0)
    {
        m_bossHp = 0;
        ChangeState(BossState::Dead);
        return;
    }

    // 피격 이펙트 표시 (깜빡임)
    m_stateTimer = 0.0f;
    isHitEffectActive = true;

    //// ✅ 공격 중엔 상태 전환 없이 이펙트만
    //if (m_state == BossState::Attack || m_state == BossState::WaitAfterAttack)
    //{
    //    // 상태 유지 (공격 도중엔 Hit 상태 전환 없음)
    //}
    //else
    //{
    //    ChangeState(BossState::Hit);  // 공격 외의 상태일 때만 Hit로 전환
    //}
}
void BossSkeleton::SetObstacleRects(const std::vector<RECT>& obstacleRects)
{
    m_obstacleRects = obstacleRects;  // m_obstacleRects는 클래스 멤버 변수로 std::vector<RECT> 타입이어야 함
}

RECT BossSkeleton::CalcBossRectAt(float x, float y)
{
    RECT rc;
    int width = static_cast<int>(120 * BOSS_SCALE);
    int height = static_cast<int>(120 * BOSS_SCALE);

    rc.left = static_cast<LONG>(x - width / 2);
    rc.right = static_cast<LONG>(x + width / 2);
    rc.top = static_cast<LONG>(y - height / 2);
    rc.bottom = static_cast<LONG>(y + height / 2);

    return rc;
}
bool BossSkeleton::CanMoveTo(float tryX, float tryY)
{
    RECT tryRect = CalcBossRectAt(tryX, tryY);

    // 장애물 리스트 순회하며 충돌 체크
    for (const RECT& obstacle : m_obstacleRects)
    {
        if (IntersectRect(nullptr, &tryRect, &obstacle))
        {
            return false;  // 충돌 발견 → 이동 불가
        }
    }

    return true;  // 모든 장애물과 충돌 없음 → 이동 가능
}

void BossSkeleton::UpdateAttackHitbox()
{
   /* OutputDebugStringA("[BossSkeleton] UpdateAttackHitbox called\n");*/
    const int attackWidth = 100;
    const int attackHeight = 100;
    const int offset = 30;
    const int shiftLeft = -15;  // 왼쪽으로 15픽셀 이동

    float centerX = m_x;
    float centerY = m_y;

    RECT rc = { 0, 0, 0, 0 };

    switch (m_direction)
    {
    case BossDirection::Left:
        rc.left = static_cast<LONG>(centerX - offset - attackWidth + shiftLeft);
        rc.right = static_cast<LONG>(centerX - offset + shiftLeft);
        rc.top = static_cast<LONG>(centerY - attackHeight / 2);
        rc.bottom = static_cast<LONG>(centerY + attackHeight / 2);
        break;

    case BossDirection::Right:
        rc.left = static_cast<LONG>(centerX + offset + shiftLeft);
        rc.right = static_cast<LONG>(centerX + offset + attackWidth + shiftLeft);
        rc.top = static_cast<LONG>(centerY - attackHeight / 2);
        rc.bottom = static_cast<LONG>(centerY + attackHeight / 2);
        break;

    case BossDirection::Up:
        rc.left = static_cast<LONG>(centerX - attackWidth / 2 + shiftLeft);
        rc.right = static_cast<LONG>(centerX + attackWidth / 2 + shiftLeft);
        rc.top = static_cast<LONG>(centerY - offset - attackHeight);
        rc.bottom = static_cast<LONG>(centerY - offset);
        break;

    case BossDirection::Down:
        rc.left = static_cast<LONG>(centerX - attackWidth / 2 + shiftLeft);
        rc.right = static_cast<LONG>(centerX + attackWidth / 2 + shiftLeft);
        rc.top = static_cast<LONG>(centerY + offset);
        rc.bottom = static_cast<LONG>(centerY + offset + attackHeight);
        break;
    }

    m_attackHitbox = rc;

  
}
RECT BossSkeleton::GetAttackHitbox() const
{
    return m_attackHitbox;
}
