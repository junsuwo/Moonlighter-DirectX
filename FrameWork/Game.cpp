
#include "Include.h"
#include <wchar.h>
#include <vector>
#include "BossSkeleton.h"
#include "GlobalSprite.h"
#include "Global.h"
#include "UIManager.h"
#include <typeinfo>
#include "SoundManager.h"
#include "Menu.h"



SoundManager soundManager;


Game game;
Menu menu;
Game* Game::s_instance = nullptr;
// 맵 관련
MapType g_currentMap = MapType::Entrance;  // 게임 시작은 입장 맵
//bool g_showColliderBox = false;
float entranceStartX = 100.f;
float entranceStartY = 100.f;

float battleStartX = 500.f;
float battleStartY = 500.f;

// 문 관련 
Door topDoor;
Door bottomDoor;

// 전역 스프라이트 리소스
LPD3DXSPRITE g_sprite = nullptr;

LPDIRECT3DDEVICE9 g_device = nullptr;
extern UIManager g_UIManager;
LPDIRECT3DTEXTURE9 g_texPotion = nullptr;
std::vector<Monster*> monsters;
BossSkeleton* boss = nullptr;

// ★ 페이드 전환용 검은 텍스처 추가
LPDIRECT3DTEXTURE9 m_blackTex = nullptr;


float GetOverlapRatio(const RECT& rc1, const RECT& rc2)
{
    LONG left = max(rc1.left, rc2.left);
    LONG right = min(rc1.right, rc2.right);
    LONG top = max(rc1.top, rc2.top);
    LONG bottom = min(rc1.bottom, rc2.bottom);

    if (right <= left || bottom <= top)
        return 0.f;

    LONG overlapWidth = right - left;
    LONG overlapHeight = bottom - top;
    LONG overlapArea = overlapWidth * overlapHeight;

    LONG rc1Area = (rc1.right - rc1.left) * (rc1.bottom - rc1.top);
    LONG rc2Area = (rc2.right - rc2.left) * (rc2.bottom - rc2.top);

    LONG minArea = min(rc1Area, rc2Area);

    return (float)overlapArea / (float)minArea;
}

bool AllMonstersDead() {
    for (auto mon : monsters) {
        if (!mon->IsDead())
            return false;
    }
    return true;
}

Game::Game()
{
}

Game::~Game()
{
    // 동적 할당된 몬스터 메모리 해제
    for (auto mon : monsters) delete mon;
    monsters.clear();

    for (auto item : dropItems) delete item;
    dropItems.clear();

    if (m_potionTex) {
        m_potionTex->Release();
        m_potionTex = nullptr;
    }
    if (m_goldTex) {
        m_goldTex->Release();
        m_goldTex = nullptr;
    }
    if (m_blackTex) { 
        m_blackTex->Release();
        m_blackTex = nullptr; }

    if (m_pBossNameFont) {
        m_pBossNameFont->Release();
        m_pBossNameFont = nullptr;
    }

    m_damageTextManager.Release();

    if (m_titleTex) {
        m_titleTex->Release();
        m_titleTex = nullptr;
    }
    if (m_endTex) {
        m_endTex->Release();
        m_endTex = nullptr;
    }
    if (m_bossDoorTex) {
        m_bossDoorTex->Release();
        m_bossDoorTex = nullptr;
    }

    if (m_pStartFont) {
        m_pStartFont->Release();
        m_pStartFont = nullptr;
    }

    // 스프라이트 해제
    if (g_sprite) {
        g_sprite->Release();
        g_sprite = nullptr;
    }
}

void Game::Init()
{
    srand((unsigned int)time(NULL));
    //g_Mng.n_Chap = MENU;     // 메뉴 챕터 설정
   /* menu.Init();*/
    s_instance = this;
    g_currentMap = MapType::Entrance;    // 현재 맵을 Entrance(0)로 설정
    map.Init(g_currentMap);              // 맵 배경 이미지 초기화
    ChangeMap(g_currentMap);             // 몬스터 초기화 + 히어로 위치 지정

    // 전역 스프라이트 객체 생성 (몬스터, UI 등 공통 사용)
    HRESULT hr = D3DXCreateSprite(g_pd3dDevice, &g_sprite);
    if (FAILED(hr)) return;

    // UIManager 초기화 + 초기 골드 설정
    soundManager.Init();
    g_UIManager.Init(g_pd3dDevice);
    g_UIManager.SetGold(0);
    m_damageTextManager.Init();

    soundManager.LoadBGM("intro", "./resource/Sound/introBGM.mp3");
    soundManager.LoadBGM("dungeon", "./resource/Sound/dungeonBGM.mp3");
    soundManager.LoadBGM("boss", "./resource/Sound/bossRoomBGM.mp3");
    soundManager.LoadBGM("town", "./resource/Sound/townBGM.mp3");       // 엔딩 BGM

    soundManager.LoadSFX("golemAttack", "./resource/Sound/golemAttack.wav");
    soundManager.LoadSFX("golemHit", "./resource/Sound/golemHit.wav");
    soundManager.LoadSFX("golemDie", "./resource/Sound/golemDie.wav");

    soundManager.LoadSFX("skeletonStep1", "./resource/Sound/skeletonStep1.wav");
    soundManager.LoadSFX("skeletonAttackHammer", "./resource/Sound/skeletonAttackHammer.wav");
    soundManager.LoadSFX("skeletonAttackSword", "./resource/Sound/skeletonAttackSword.wav");

    soundManager.LoadSFX("potion", "./resource/Sound/potion.wav");
    soundManager.LoadSFX("heal", "./resource/Sound/heal.wav");
    soundManager.LoadSFX("roll", "./resource/Sound/roll.wav");
    soundManager.LoadSFX("walk", "./resource/Sound/walk.wav");
    soundManager.LoadSFX("Sword", "./resource/Sound/Sword.wav");


    ChangeMap(MapType::Entrance);
    // 문(door) 초기화 및 위치 지정
    topDoor.Init("./resource/Img/door/doorfront.png", true);   // 위쪽 문
    bottomDoor.Init("./resource/Img/door/doorfront.png", false); // 아래쪽 문
    topDoor.SetPos((SCREEN_RIGHT - DOOR_FRAME_W) / 2.f, 0.f);
    bottomDoor.SetPos((SCREEN_RIGHT - DOOR_FRAME_W) / 2.f, SCREEN_BOTTOM - DOOR_FRAME_H);

    // 보스 도어 텍스처 로드 후 세팅 예시
    LPDIRECT3DTEXTURE9 bossDoorTex = nullptr;
    hr = D3DXCreateTextureFromFileA(g_pd3dDevice, "./resource/Img/door/bossDoorTop.png", &m_bossDoorTex);
    if (FAILED(hr)) {
        m_bossDoorTex = nullptr;  // 실패시 nullptr 처리
    }

    // 드롭 아이템 텍스처 로드
    hr = D3DXCreateTextureFromFileA(g_pd3dDevice, "./resource/Img/Item/potion_B.png", &m_potionTex);
    if (m_potionTex)
        g_UIManager.SetPotionTexture(m_potionTex);

    hr = D3DXCreateTextureFromFileA(g_pd3dDevice, "./resource/Img/Item/gold01.png", &m_goldTex);


    // ★ 페이드 전환용 검은 텍스처 생성
    hr = D3DXCreateTexture(g_pd3dDevice, SCREEN_RIGHT, SCREEN_BOTTOM, 1, 0,
        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_blackTex);
    if (SUCCEEDED(hr)) {
        D3DLOCKED_RECT lr;
        if (SUCCEEDED(m_blackTex->LockRect(0, &lr, nullptr, 0))) {
            DWORD* pixels = (DWORD*)lr.pBits;
            for (int i = 0; i < SCREEN_RIGHT * SCREEN_BOTTOM; ++i)
                pixels[i] = D3DCOLOR_ARGB(255, 0, 0, 0); // 완전검정
            m_blackTex->UnlockRect(0);
        }
    }
    // 보스 이름 폰트 생성
    D3DXFONT_DESC fontDesc = {};
    fontDesc.Height = 24;
    fontDesc.Width = 0;
    fontDesc.Weight = FW_BOLD;
    fontDesc.MipLevels = 1;
    fontDesc.Italic = false;
    fontDesc.CharSet = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    strcpy_s(fontDesc.FaceName, LF_FACESIZE, "Arial");

    // 타이틀 시작 문구용 폰트 생성
    D3DXFONT_DESC startFontDesc = {};
    startFontDesc.Height = 48;
    startFontDesc.Width = 0;
    startFontDesc.Weight = FW_NORMAL;
    startFontDesc.MipLevels = 1;
    startFontDesc.Italic = false;
    startFontDesc.CharSet = DEFAULT_CHARSET;
    startFontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    startFontDesc.Quality = DEFAULT_QUALITY;
    startFontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
    strcpy_s(startFontDesc.FaceName, LF_FACESIZE, "Arial");

    hr = D3DXCreateFontIndirect(g_pd3dDevice, &startFontDesc, &m_pStartFont);
    if (FAILED(hr)) {
        m_pStartFont = nullptr;
        OutputDebugStringA("StartFont 생성 실패\n");
    }
    else {
        OutputDebugStringA("StartFont 생성 성공\n");
    }


     hr = D3DXCreateFontIndirect(g_pd3dDevice, &fontDesc, &m_pBossNameFont);
    if (FAILED(hr)) {
        m_pBossNameFont = nullptr;
        OutputDebugStringA("BossNameFont 생성 실패\n");
    }
    else {
        OutputDebugStringA("BossNameFont 생성 성공\n");
    }

    // 타이틀 이미지 로드
    hr = D3DXCreateTextureFromFileA(g_pd3dDevice, "./resource/Img/lobby/start2.png", &m_titleTex);
    if (FAILED(hr)) {
        m_titleTex = nullptr;
        OutputDebugStringA("타이틀 이미지 로드 실패\n");
    }

    // 엔딩 이미지 로드
    hr = D3DXCreateTextureFromFileA(g_pd3dDevice, "./resource/Img/lobby/END.png", &m_endTex);
    if (FAILED(hr)) {
        m_endTex = nullptr;
        OutputDebugStringA("엔딩 이미지 로드 실패\n");
    }


    g_UIManager.Init(g_pd3dDevice);
    // 히어로 이미지 로드 및 초기화
    hero.Init("./resource/Img/Hero/moveState.png");

    // 기타 시스템 초기화
    coll.Init();
    /*sound.Init();*/
    Gmanager.Init();
    //sql.Init();
}

void Game::Draw()
{

    if (m_gameState == GameState::Title)
    {
        if (m_titleTex)
        {
            D3DSURFACE_DESC desc;
            m_titleTex->GetLevelDesc(0, &desc);
            float imgWidth = (float)desc.Width;
            float imgHeight = (float)desc.Height;

            float screenW = (float)SCREEN_RIGHT;
            float screenH = (float)SCREEN_BOTTOM;

            float scaleX = screenW / imgWidth;
            float scaleY = screenH / imgHeight;

            D3DXVECTOR3 center(0, 0, 0);
            D3DXVECTOR3 pos(0, 0, 0);

            g_sprite->Begin(D3DXSPRITE_ALPHABLEND);

            D3DXMATRIX matScale, matTrans, matWorld;
            D3DXMatrixScaling(&matScale, scaleX, scaleY, 1);
            D3DXMatrixTranslation(&matTrans, pos.x, pos.y, 0);
            matWorld = matScale * matTrans;

            g_sprite->SetTransform(&matWorld);
            g_sprite->Draw(m_titleTex, nullptr, &center, nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
            g_sprite->End();

            D3DXMATRIX identity;
            D3DXMatrixIdentity(&identity);
            g_sprite->SetTransform(&identity);
        }

        // 깜빡임 텍스트 출력
        float interval = 0.1f;  // 1초 주기
        float halfInterval = interval / 2.0f;
        float timerMod = fmod(m_startTextBlinkTimer, interval);

        if (timerMod < halfInterval)
        {
            if (m_pStartFont)
            {
                RECT textRect = { 0, 730, SCREEN_RIGHT, 790 };
                const wchar_t* text = L"게임을 시작하려면 마우스 왼쪽 버튼을 누르십시오";
                m_pStartFont->DrawTextW(
                    nullptr, text, -1, &textRect,
                    DT_CENTER | DT_SINGLELINE | DT_NOCLIP,
                    D3DCOLOR_ARGB(255, 255, 255, 255)
                );
            }
        }

        return;  // 타이틀 화면은 여기서 종료
    }

    if (m_gameState == GameState::End)
    {
        if (m_endTex)
        {
            D3DSURFACE_DESC desc;
            m_endTex->GetLevelDesc(0, &desc);
            float imgWidth = (float)desc.Width;
            float imgHeight = (float)desc.Height;

            float screenW = (float)SCREEN_RIGHT;
            float screenH = (float)SCREEN_BOTTOM;

            float scaleX = screenW / imgWidth;
            float scaleY = screenH / imgHeight;
            float scale = min(scaleX, scaleY);

            D3DXVECTOR3 center(imgWidth / 2.f, imgHeight / 2.f, 0);
            D3DXVECTOR3 pos(screenW / 2.f, screenH / 2.f, 0);

            g_sprite->Begin(D3DXSPRITE_ALPHABLEND);
            D3DXMATRIX matScale, matTrans, matWorld;
            D3DXMatrixScaling(&matScale, scale, scale, 1);
            D3DXMatrixTranslation(&matTrans, pos.x, pos.y, 0);
            matWorld = matScale * matTrans;

            g_sprite->SetTransform(&matWorld);
            g_sprite->Draw(m_endTex, nullptr, &center, nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));
            g_sprite->End();

            D3DXMATRIX identity;
            D3DXMatrixIdentity(&identity);
            g_sprite->SetTransform(&identity);
        }
        return;  // 엔딩 화면 종료
    }

    // Playing 상태 그리기

    Camera* cam = Camera::GetInstance();
    cam->Update();
    D3DXMATRIX camMat;
    cam->GetTransformMatrix(&camMat);

    if (g_sprite) {
        g_sprite->SetTransform(&camMat);
    }

    map.Draw();
    m_damageTextManager.Render();

    for (auto* mon : monsters) mon->Draw();

    if (boss) boss->Draw();

    for (auto* item : dropItems) item->Render(g_sprite);

    coll.Draw();

    if (g_currentMap != MapType::Boss) {
        topDoor.Draw();
        bottomDoor.Draw();
    }

    hero.Draw();
    Gmanager.Draw();
    //sql.Draw();

    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);

    if (g_sprite) {
        g_sprite->SetTransform(&identity);
    }

    g_UIManager.Render();

    if (boss && !boss->IsDead()) {
        g_UIManager.DrawBossHpBar(boss->GetBossHP(), boss->GetBossMaxHP(), 90, 700);

        if (m_pBossNameFont) {
            RECT textRect = { -30, 670, -30 + 400, 670 + 30 };
            const wchar_t* bossName = L"듀얼웨폰스켈레톤";
            m_pBossNameFont->DrawTextW(
                nullptr, bossName, -1, &textRect,
                DT_CENTER | DT_SINGLELINE | DT_NOCLIP,
                D3DCOLOR_ARGB(255, 255, 255, 255)
            );
        }
    }

    if (m_isFading && m_blackTex) {
        g_sprite->Begin(D3DXSPRITE_ALPHABLEND);
        D3DXCOLOR fadeColor = D3DCOLOR_ARGB(m_fadeAlpha, 0, 0, 0);
        g_sprite->Draw(m_blackTex, nullptr, nullptr, nullptr, fadeColor);
        g_sprite->End();
    }

    if (g_showColliderBox && g_currentMap != MapType::Boss) {
        coll.BoxSow(topDoor.GetTriggerRc(), 0, 0, D3DCOLOR_ARGB(255, 255, 255, 0));
        coll.BoxSow(bottomDoor.GetTriggerRc(), 0, 0, D3DCOLOR_ARGB(255, 255, 255, 0));
    }
}
void Game::Update(double frame)
{
    key.Update();
    static ULONGLONG lastTime = 0;
    ULONGLONG now = GetTickCount64();

    float deltaTime = static_cast<float>(frame) / 1000.f;

    // 상태별 BGM 재생 플래그
    static bool introBGMPlayed = false;
    static bool townBGMPlayed = false;

    // Title 상태 처리
    if (m_gameState == GameState::Title)
    {
        m_startTextBlinkTimer += deltaTime;

        // intro BGM 재생
        if (!introBGMPlayed) {
            soundManager.StopBGM();
            soundManager.PlayBGM("intro");
            introBGMPlayed = true;
            townBGMPlayed = false;
        }

        if (key.Down(VK_LBUTTON))
        {
            ChangeMap(MapType::Entrance);
            m_gameState = GameState::Playing;

            // 던전 BGM으로 교체
            soundManager.StopBGM();
            soundManager.PlayBGM("dungeon");
        }

        return;
    }

    // End 상태 처리
    if (m_gameState == GameState::End)
    {
        // town BGM 재생
        if (!townBGMPlayed) {
            soundManager.StopBGM();
            soundManager.PlayBGM("town");
            townBGMPlayed = true;
            introBGMPlayed = false;
        }

        if (key.Down(VK_LBUTTON)) {
            PostQuitMessage(0);
        }

        return;
    }

    // Playing 상태일 경우 intro/town BGM 재생 플래그 초기화
    if (m_gameState == GameState::Playing) {
        introBGMPlayed = false;
        townBGMPlayed = false;
    }

    soundManager.Update();

    if (m_isFading) {
        UpdateFadeTransition(frame);
        return;
    }

    // 단축키
    if (key.Down('E')) StartFadeTransition(MapType::Battle);
    if (key.Down('Q')) StartFadeTransition(MapType::Entrance);
    if (key.Down('R')) StartFadeTransition(MapType::Boss);

    static bool spacePressed = false;
    if (key.Down(VK_SPACE) && !spacePressed && !hero.IsAttacking()) {
        hero.StartRoll();
        spacePressed = true;
    }
    else if (!key.Pressing(VK_SPACE)) {
        spacePressed = false;
    }

    if (key.Pressed('J') && g_UIManager.IsInventoryOpen()) {
        g_UIManager.UseSelectedItem(hero);
    }

    if (now - lastTime > frame)
    {
        UpdateFadeTransition(deltaTime);
        if (m_isFading) return;

        m_damageTextManager.Update(deltaTime);

        if (!m_ignoreDoorCollision) {
            RECT heroRc = hero.m_rc;
            RECT tempRc1;
            if (IntersectRect(&tempRc1, &heroRc, &topDoor.GetTriggerRc()) && topDoor.GetState() == DoorState::OPENED) {
                m_ignoreDoorCollision = true;
                m_ignoreDoorEndTime = now + 1500;
                StartFadeTransition(MapType::Battle);
            }
            else {
                RECT tempRc2;
                if (IntersectRect(&tempRc2, &heroRc, &bottomDoor.GetTriggerRc()) && bottomDoor.GetState() == DoorState::OPENED) {
                    m_ignoreDoorCollision = true;
                    m_ignoreDoorEndTime = now + 1500;
                    StartFadeTransition(MapType::Entrance);
                }
            }
        }

        if (g_currentMap == MapType::Battle) {
            RECT heroRc = hero.m_rc;
            RECT temp;
            if (IntersectRect(&temp, &heroRc, &m_bossDoorTriggerRc)) {
                if (topDoor.GetState() == DoorState::OPENED && topDoor.IsBossDoor()) {
                    StartFadeTransition(MapType::Boss);
                }
            }
        }

        if (m_ignoreDoorCollision && now > m_ignoreDoorEndTime) {
            m_ignoreDoorCollision = false;
        }

        hero.Update();
        g_UIManager.SetHP(hero.GetHp(), hero.m_maxHP);

        if (g_UIManager.IsInventoryOpen()) {
            g_UIManager.UpdateInventoryCursor();
        }

        std::vector<RECT> obstacles;
        obstacles.push_back(topDoor.GetTriggerRc());
        obstacles.push_back(bottomDoor.GetTriggerRc());

        for (auto* mon : monsters) {
            mon->SetPlayerPos(hero.GetCenterX(), hero.GetCenterY());
            mon->SetHeroCollisionBox(hero.m_rc);
            mon->SetObstacleRects(obstacles);
            mon->Update(deltaTime);

            if (mon->IsDead() && !mon->m_dropCreated) {
                SpawnDropItems(mon->GetX(), mon->GetY());
                mon->m_dropCreated = true;
            }
        }

        if (boss) {
            boss->SetPlayerPos(hero.GetCenterX(), hero.GetCenterY());
            boss->SetHeroCollisionBox(hero.m_rc);
            boss->SetObstacleRects(obstacles);
            boss->Update(deltaTime);

            if (boss->IsDead() && !boss->m_dropCreated) {
                SpawnDropItems(boss->GetX(), boss->GetY());
                boss->m_dropCreated = true;
            }
        }

        if (boss && boss->IsDead()) {
            m_gameState = GameState::End;
        }

        if (AllMonstersDead()) {
            topDoor.TriggerOpen();
            bottomDoor.TriggerOpen();
        }

        if (g_currentMap != MapType::Boss) {
            topDoor.Update();
            bottomDoor.Update();
        }

        for (auto it = dropItems.begin(); it != dropItems.end(); )
        {
            DropItem* item = *it;
            item->Update(deltaTime);

            if (item->IsCollected() && !item->IsFlying()) {
                if (item->GetType() == DropType::GOLD)
                    g_UIManager.AddGold(100);
                else if (item->GetType() == DropType::HP_POTION)
                    g_UIManager.AddPotion(1);

                g_UIManager.AddItemToInventory(item->GetType(), 1);

                delete item;
                it = dropItems.erase(it);
            }
            else if (item->CheckCollisionWithHero(hero.m_rc) && !item->IsFlying()) {
                D3DXVECTOR2 uiPos = g_UIManager.GetInventorySlotPosition(item->GetType());
                item->StartFlyToUI(uiPos);
                item->m_collected = true;
                ++it;
            }
            else {
                ++it;
            }
        }

        coll.Update();
        map.Update(130);
        Gmanager.Update();
        //sql.Update(frame + 3000);

        if (key.Down('Z')) {
            topDoor.TriggerOpen();
            bottomDoor.TriggerOpen();
        }

        static bool prevL = false;
        bool currL = GetAsyncKeyState('L') & 0x8000;
        if (currL && !prevL) {
            g_showColliderBox = !g_showColliderBox;
            OutputDebugStringA(g_showColliderBox ? "[디버그] 충돌 박스 ON\n" : "[디버그] 충돌 박스 OFF\n");
        }
        prevL = currL;

        lastTime = now;
    }
}

void Game::OnMessage(MSG* msg)
{
}

void Game::SpawnDropItems(float x, float y)
{
    float monsterFootY = y + 280;  //  몬스터 발바닥 기준 y좌표

    //  오프셋 적용: 원하는 만큼 조절
    float offsetX_Gold = +130;   // 왼쪽으로 10
    float offsetY_Gold = -20;   // 위쪽에서 떨어지기

    float offsetX_Potion = +160;   // 오른쪽으로 10
    float offsetY_Potion = -20;   // 위쪽에서 떨어지기

    dropItems.push_back(new DropItem(
        DropType::GOLD,
        x + offsetX_Gold,                  //  오프셋 적용된 X
        monsterFootY + offsetY_Gold,       //  오프셋 적용된 Y
        m_goldTex,
        monsterFootY));                    // 바닥 기준

    dropItems.push_back(new DropItem(
        DropType::HP_POTION,
        x + offsetX_Potion,                // 오프셋 적용된 X
        monsterFootY + offsetY_Potion,     //  오프셋 적용된 Y
        m_potionTex,
        monsterFootY));                    // 바닥 기준
}

void Game::ChangeMap(MapType newMap)
{
    g_currentMap = newMap;
    map.Init(g_currentMap);
    m_obstacleRects = map.GetObstacleRects();

    // 몬스터 정리
    for (auto m : monsters) delete m;
    monsters.clear();

    // 보스 해제 (보스방이 아닐 때만)
    if (boss && newMap != MapType::Boss) {
        delete boss;
        boss = nullptr;
        hero.boss = nullptr;
    }

    // 도어 초기화
    topDoor.TriggerClose();
    bottomDoor.TriggerClose();
    SetRectEmpty(&m_bossDoorTriggerRc);

    // 히어로 시작 위치 설정
    float entranceStartX = 580.f, entranceStartY = 600.f;
    float battleStartX = 580.f, battleStartY = 600.f;
    float bossStartX = 580.f, bossStartY = 600.f;

    if (g_currentMap == MapType::Entrance) {
        hero.SetPos(entranceStartX, entranceStartY);
        topDoor.SetBossDoorTexture(nullptr);  // 보스 도어 숨김
    }

    else if (g_currentMap == MapType::Battle) {
        hero.SetPos(battleStartX, battleStartY);
        topDoor.SetBossDoorTexture(m_bossDoorTex);  // 보스 도어 표시

        // 도어 충돌 영역 설정
        float x = topDoor.GetPosX();
        float y = topDoor.GetPosY();
        m_bossDoorTriggerRc = {
            static_cast<LONG>(x),
            static_cast<LONG>(y),
            static_cast<LONG>(x + DOOR_FRAME_W),
            static_cast<LONG>(y + DOOR_FRAME_H)
        };

        std::vector<std::pair<float, float>> spawnPositions = {
            {100.f, 100.f}, {100.f, 300.f},
            {450.f, 100.f}, {450.f, 300.f}
        };

        for (const auto& pos : spawnPositions) {
            float mx = pos.first;
            float my = pos.second;

            Monster* mon = new Monster(mx, my, g_sprite);
            mon->SetHero(&hero);
            mon->SetObstacleRects(m_obstacleRects);
            monsters.push_back(mon);
        }
    }

    else if (g_currentMap == MapType::Boss) {
        hero.SetPos(bossStartX, bossStartY);
        topDoor.SetBossDoorTexture(nullptr);  // 도어 숨김

        // 보스 생성
        boss = new BossSkeleton(650.f, 300.f, g_sprite);
        boss->SetHero(&hero);
        boss->SetObstacleRects(m_obstacleRects);
        hero.boss = boss;
    }
    if (m_gameState == GameState::Playing) {
        switch (g_currentMap)
        {
        case MapType::Entrance:
        case MapType::Battle:
            soundManager.PlayBGM("dungeon");
            break;
        case MapType::Boss:
            soundManager.PlayBGM("boss");
            break;
        default:
            soundManager.StopBGM();
            break;
        }
    }
}

// ★ 페이드 전환 시작
void Game::StartFadeTransition(MapType nextMap)
{

    // 다음 맵 가기 전 드롭 아이템 제거
    ClearDropItems();

    m_isFading = true;
    m_fadeAlpha = 0;
    m_fadeDirection = 1; // 아웃부터
    m_fadeNextMap = nextMap;


}


template<typename T>
T clamp(const T& value, const T& low, const T& high)
{
    return (value < low) ? low : (value > high) ? high : value;
}


// ★ 페이드 전환 상태 갱신
void Game::UpdateFadeTransition(double frame)
{
    if (!m_isFading) return;

    // float 계산, 부드럽고 일정한 속도로 변경
    float fadeSpeed = static_cast<float>(frame) * 255.f * 0.3f;

    if (m_fadeDirection == 1) {
        m_fadeAlpha += fadeSpeed;
    }
    else if (m_fadeDirection == -1) {
        m_fadeAlpha -= fadeSpeed;
    }

    // clamp도 float용으로 변경
    m_fadeAlpha = clamp<float>(m_fadeAlpha, 0.f, 255.f);

    if (m_fadeDirection == 1 && m_fadeAlpha >= 255.f) {
        ChangeMap(m_fadeNextMap);       // 맵 교체

        // 맵 전환 후 보스 도어 충돌 영역 초기화
        if (g_currentMap == MapType::Battle) {
            // topDoor 위치와 크기 기준으로 보스 도어 충돌 영역 설정
            float x = topDoor.GetPosX();  // topDoor 위치 얻는 함수 필요
            float y = topDoor.GetPosY();
            float width = DOOR_FRAME_W;
            float height = DOOR_FRAME_H;

            m_bossDoorTriggerRc.left = static_cast<LONG>(x);
            m_bossDoorTriggerRc.top = static_cast<LONG>(y);
            m_bossDoorTriggerRc.right = static_cast<LONG>(x + width);
            m_bossDoorTriggerRc.bottom = static_cast<LONG>(y + height);
        }
        else {
            SetRectEmpty(&m_bossDoorTriggerRc);
        }

        m_fadeDirection = -1;           // 인으로 전환
    }
    else if (m_fadeDirection == -1 && m_fadeAlpha <= 0.f) {
        m_isFading = false;             // 전환 완료
    }
}

void Game::ClearDropItems()
{
    for (auto item : dropItems) {
        delete item;
    }
    dropItems.clear();
}


void Game::EnterBossRoom()
{
    Camera* cam = Camera::GetInstance();
    OutputDebugStringA("EnterBossRoom 시작\n");

    if (boss) boss->ChangeState(BossState::Chase);

    // 1) 확대 전 카메라 줌을 1.5로 바로 설정 (확대 적용)
    cam->SetScale(1.5f);
    OutputDebugStringA("Camera scale set to 1.50\n");

    // 2) 확대 적용된 상태(위치, 스케일) 저장
    m_originalCamPosX = cam->GetPosX();
    m_originalCamPosY = cam->GetPosY();
    m_originalCamScale = cam->GetScale();

    char buf[128];
    sprintf(buf, "Original camera scale stored: %.2f\n", m_originalCamScale);
    OutputDebugStringA(buf);

    // 3) 흔들림 시작을 알리고 타이머 초기화
    m_cameraShaking = true;
    m_cameraShakeTimer = 0.0f;
}
