#define NOMINMAX   // ✅ windows.h가 max/min 매크로 정의 못하게 막음
#include"Include.h"
#include <windows.h>
#include <algorithm>  // ✅ 이건 std::max 사용 위해 반드시 필요
#include"UIManager.h"
#include"Global.h"

using std::max;
using std::min;

UIManager g_UIManager;


// 전역 g_sprite를 사용하기 위해 extern 선언
extern LPD3DXSPRITE g_sprite;
extern LPDIRECT3DTEXTURE9 g_texPotion; // 포션 텍스처 전역변수 (Game.cpp에서 로드)

UIManager::UIManager()
{
    m_gold = 0;
    m_potionCount = 0;
    m_inventoryOpen = false;
    m_selectedSlotIndex = 0;
}

UIManager::~UIManager()
{
    Release();
}

void UIManager::Init(LPDIRECT3DDEVICE9 device)
{
    /*OutputDebugStringA("UIManager::Init() 시작\n");*/

    m_device = device;
    if (m_device)
        m_device->AddRef();


    // 전역 g_sprite를 dv_font.Sprite에 연결
    dv_font.Sprite = g_sprite;

    HRESULT hr;

    // 체력바 배경 이미지 로딩
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/hp_barBack.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_hpBarBackTex);
   /* if (FAILED(hr)) OutputDebugStringA("hp_barBack 텍스처 로딩 실패!\n");
    else OutputDebugStringA("hp_barBack 텍스처 로딩 성공!\n");*/
    m_hpBarBackSprite.Texture = m_hpBarBackTex;

    // 체력바 전면 이미지 로딩
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/hp_barFront.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_hpBarFrontTex);
    /*if (FAILED(hr)) OutputDebugStringA("hp_barFront 텍스처 로딩 실패!\n");
    else OutputDebugStringA("hp_barFront 텍스처 로딩 성공!\n");*/
    m_hpBarFrontSprite.Texture = m_hpBarFrontTex;

    // UI_gold.png 로딩
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/UI_gold.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_goldBackTex);
    if (FAILED(hr)) OutputDebugStringA("UI_gold 텍스처 로딩 실패!\n");
    else OutputDebugStringA("UI_gold 텍스처 로딩 성공!\n");
    m_goldBackSprite.Texture = m_goldBackTex;

    // money_bag.png 로딩
    for (int i = 0; i < 6; ++i) {
        char filename[128];
        sprintf_s(filename, "./resource/Img/ui/UI_gold%03d.png", i);
        HRESULT hr = D3DXCreateTextureFromFileExA(m_device, filename,
            D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
            D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
            D3DX_DEFAULT, D3DX_DEFAULT,
            D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
            &m_goldFrames[i]);

        if (FAILED(hr)) {
            OutputDebugStringA("[UIManager] UI_gold 텍스처 로딩 실패!\n");
            // 필요시 오류 처리
        }
        else {
            m_goldFrameSprites[i].Texture = m_goldFrames[i];
            m_goldFrameSprites[i].imagesinfo.Width = 70;
            m_goldFrameSprites[i].imagesinfo.Height = 73;
        }
    }
    // 보스 체력바 배경 이미지 로딩 (화이트)
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/boss/ui/semiBossHpFront(white).png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_bossHpBarBackTex);
    if (FAILED(hr)) OutputDebugStringA("[UIManager] 보스 체력바 배경 텍스처 로딩 실패!\n");

    m_bossHpBarBackSprite.Texture = m_bossHpBarBackTex;

    // 보스 체력바 전면 이미지 로딩 (레드)
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/boss/ui/semiBossHpFront(red).png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_bossHpBarFrontTex);
    if (FAILED(hr)) OutputDebugStringA("[UIManager] 보스 체력바 전면 텍스처 로딩 실패!\n");

    m_bossHpBarFrontSprite.Texture = m_bossHpBarFrontTex;

    // UI_hart.png 로딩
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/UI_hart.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_hartBackTex);
    if (FAILED(hr)) OutputDebugStringA("UI_hart 텍스처 로딩 실패!\n");
    else OutputDebugStringA("UI_hart 텍스처 로딩 성공!\n");

    m_hartBackSprite.Texture = m_hartBackTex;

    // UI_right_top 로딩
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/UI_right_top.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_rightTopTex);

    m_rightTopSprite.Texture = m_rightTopTex;

    // UI_right_bot 로딩
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/UI_right_bot.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_rightBotTex);

    m_rightBotSprite.Texture = m_rightBotTex;

    // --- 여기서 인벤토리 텍스처 로딩 추가 ---

    hr = D3DXCreateTextureFromFileExA(
        m_device,
        "./resource/Img/ui/menu_inventory.png",
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255),
        nullptr, nullptr,
        &m_inventoryTex
    );
    InitInventorySlots();

    if (FAILED(hr)) {
        /*OutputDebugStringA("[UIManager] 인벤토리 텍스처 로딩 실패!\n");*/
    }
    else {
        /*OutputDebugStringA("[UIManager] 인벤토리 텍스처 로딩 성공!\n");*/
        m_inventorySprite.Texture = m_inventoryTex;

        // 인벤토리 이미지 크기 저장
        D3DSURFACE_DESC desc;
        m_inventoryTex->GetLevelDesc(0, &desc);
        m_inventorySprite.imagesinfo.Width = desc.Width;
        m_inventorySprite.imagesinfo.Height = desc.Height;
    }

    // 커서 텍스처 로딩
    HRESULT cursorHr = D3DXCreateTextureFromFileExA(
        m_device,
        "./resource/Img/ui/invenSlot.png", // → 테두리 이미지 예: 빨간 네모
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_cursorTex
    );

    if (SUCCEEDED(cursorHr)) {
        m_cursorSprite.Texture = m_cursorTex;

        D3DSURFACE_DESC desc;
        m_cursorTex->GetLevelDesc(0, &desc);
        m_cursorSprite.imagesinfo.Width = desc.Width;
        m_cursorSprite.imagesinfo.Height = desc.Height;
    }

    // --- 포션 텍스처 직접 로딩 추가 ---
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/Item/potion_B.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_potionTex);

    if (FAILED(hr)) {
        OutputDebugStringA("UIManager::Init - 포션 텍스처 로딩 실패! 경로 및 파일명 확인 필요\n");
        m_potionTex = nullptr;
    }
    else {
        OutputDebugStringA("UIManager::Init - 포션 텍스처 로딩 성공\n");
    }

    // item_name 배경 이미지 로딩 (700x54)
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/item_name.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_itemNameTex);
    if (FAILED(hr)) {
        OutputDebugStringA("UIManager::Init - item_name 텍스처 로딩 실패!\n");
        m_itemNameTex = nullptr;
    }
    else {
        OutputDebugStringA("UIManager::Init - item_name 텍스처 로딩 성공\n");
    }

    if (m_itemNameTex)
    {
        D3DSURFACE_DESC desc;
        m_itemNameTex->GetLevelDesc(0, &desc);
        // 필요한 Sprite 객체 있으면 사이즈 세팅 또는 멤버 변수에 저장
    }

    // name_hpPotion1 텍스트 이미지 로딩 (160x24)
    hr = D3DXCreateTextureFromFileExA(m_device, "./resource/Img/ui/name_hpPotion1.png",
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_UNKNOWN, D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DCOLOR_XRGB(255, 0, 255), nullptr, nullptr,
        &m_nameHpPotionTex);
    if (FAILED(hr)) {
        OutputDebugStringA("UIManager::Init - name_hpPotion1 텍스처 로딩 실패!\n");
        m_nameHpPotionTex = nullptr;
    }
    else {
        OutputDebugStringA("UIManager::Init - name_hpPotion1 텍스처 로딩 성공\n");
    }
    if (m_nameHpPotionTex)
    {
        D3DSURFACE_DESC desc;
        m_nameHpPotionTex->GetLevelDesc(0, &desc);
    }
    if (m_bossHpBarBackTex)
    {
        D3DSURFACE_DESC desc;
        m_bossHpBarBackTex->GetLevelDesc(0, &desc);
        m_bossHpBarBackSprite.imagesinfo.Width = desc.Width;
        m_bossHpBarBackSprite.imagesinfo.Height = desc.Height;
    }

    if (m_bossHpBarFrontTex)
    {
        D3DSURFACE_DESC desc;
        m_bossHpBarFrontTex->GetLevelDesc(0, &desc);
        m_bossHpBarFrontSprite.imagesinfo.Width = desc.Width;
        m_bossHpBarFrontSprite.imagesinfo.Height = desc.Height;
    }




    // 이미지 크기 저장
    D3DSURFACE_DESC desc;
    m_hpBarFrontTex->GetLevelDesc(0, &desc);
    m_hpBarFrontSprite.imagesinfo.Width = desc.Width;
    m_hpBarFrontSprite.imagesinfo.Height = desc.Height;

    m_hpBarBackTex->GetLevelDesc(0, &desc);
    m_hpBarBackSprite.imagesinfo.Width = desc.Width;
    m_hpBarBackSprite.imagesinfo.Height = desc.Height;

    m_goldBackTex->GetLevelDesc(0, &desc);
    m_goldBackSprite.imagesinfo.Width = desc.Width;
    m_goldBackSprite.imagesinfo.Height = desc.Height;

    m_hartBackTex->GetLevelDesc(0, &desc);
    m_hartBackSprite.imagesinfo.Width = desc.Width;
    m_hartBackSprite.imagesinfo.Height = desc.Height;

    m_rightTopTex->GetLevelDesc(0, &desc);
    m_rightTopSprite.imagesinfo.Width = desc.Width;
    m_rightTopSprite.imagesinfo.Height = desc.Height;

    m_rightBotTex->GetLevelDesc(0, &desc);
    m_rightBotSprite.imagesinfo.Width = desc.Width;
    m_rightBotSprite.imagesinfo.Height = desc.Height;


    D3DXFONT_DESCA fontDesc = {};
    fontDesc.Height = 16; // 폰트 크기
    fontDesc.Weight = FW_BOLD;
    fontDesc.CharSet = HANGUL_CHARSET;
    fontDesc.Quality = ANTIALIASED_QUALITY;
    strcpy_s(fontDesc.FaceName, "맑은 고딕");  // 또는 "Arial"

    HRESULT fontHr = D3DXCreateFontIndirectA(m_device, &fontDesc, &m_smallFont);
    if (FAILED(fontHr)) {
        /*OutputDebugStringA("[UIManager] 작은 폰트 생성 실패!\n");*/
    }
}

void UIManager::Release()
{
    if (m_hpBarBackTex) { m_hpBarBackTex->Release(); m_hpBarBackTex = nullptr; }
    if (m_hpBarFrontTex) { m_hpBarFrontTex->Release(); m_hpBarFrontTex = nullptr; }

    // 전역 g_sprite는 UIManager에서 해제하지 않음
    dv_font.Sprite = nullptr;

    if (m_device) {
        m_device->Release();
        m_device = nullptr;
    }
    if (m_smallFont) {
        m_smallFont->Release();
        m_smallFont = nullptr;
    }
    if (m_blackTex) {
        m_blackTex->Release();
        m_blackTex = nullptr;
    }
    for (int i = 0; i < 6; ++i) {
        if (m_goldFrames[i]) {
            m_goldFrames[i]->Release();
            m_goldFrames[i] = nullptr;
        }
    }
    if (m_hartBackTex) { 
        m_hartBackTex->Release();
        m_hartBackTex = nullptr; }

    if (m_rightTopTex) {
        m_rightTopTex->Release(); 
        m_rightTopTex = nullptr; }

    if (m_rightBotTex) {
        m_rightBotTex->Release();
        m_rightBotTex = nullptr; }

    if (m_inventoryTex) {
        m_inventoryTex->Release();
        m_inventoryTex = nullptr;
    }

}

void UIManager::SetHP(int currentHP, int maxHP) {
    // 최소 보정
    m_currentHP = std::max(0, currentHP);
    m_maxHP = std::max(1, maxHP);
    /*char buffer[64];
    sprintf_s(buffer, "UIManager::SetHP 호출 - currentHP: %d, maxHP: %d\n", m_currentHP, m_maxHP);
    OutputDebugStringA(buffer);*/
}

void UIManager::SetGold(int gold)
{
    m_gold = gold;
}

void UIManager::AddGold(int amount)
{
    m_gold += amount;
}
int UIManager::GetGold() const
{
    return m_gold;
}
void UIManager::AddPotion(int amount)
{
    m_potionCount += amount;
}
int UIManager::GetPotionCount() const
{
    return m_potionCount;
}


void UIManager::Render()
{
    /*OutputDebugStringA("[UIManager] 체력바 렌더 호출됨\n");*/
    if (!m_hpBarBackTex || !m_hpBarFrontTex || !dv_font.Sprite) {
        OutputDebugStringA("[UIManager] 텍스처 또는 스프라이트 없음, 렌더링 중지\n");
        return;
    }

    // 체력바 비율 계산 (0.0 ~ 1.0)
    float hpRatio = std::max(0.0f, std::min(1.0f, (float)m_currentHP / m_maxHP));

    // --- 체력바 영역 ---
    float hpTexW = (float)m_hpBarFrontSprite.imagesinfo.Width;
    float hpTexH = (float)m_hpBarFrontSprite.imagesinfo.Height;

    float hpScaleX = 0.45f;
    float hpScaleY = 0.6f;
    float hpBarX = 120.0f;
    float hpBarY = 10.0f;

    // 체력바 배경, 전면: RenderDraw 내부에서 Begin/End 처리됨
    m_hpBarBackSprite.SetColor(255, 255, 255, 255);
    m_hpBarBackSprite.RenderDraw(hpBarX, hpBarY, 0.f, 0.f, hpTexW, hpTexH, 0.f, hpScaleX, hpScaleY);

    m_hpBarFrontSprite.SetColor(255, 255, 255, 255);
    m_hpBarFrontSprite.RenderDraw(hpBarX, hpBarY, 0.f, 0.f, hpTexW * hpRatio, hpTexH, 0.f, hpScaleX, hpScaleY);

    // 체력 텍스트 표시 (예: "100 / 100")
    char hpStr[32];
    sprintf_s(hpStr, "%d / %d  ", m_currentHP, m_maxHP);

    if (m_smallFont && g_sprite)
    {
        // 텍스트만 별도 Begin/End 처리
        g_sprite->Begin(D3DXSPRITE_ALPHABLEND);

        RECT hpTextRc = {
            (int)(hpBarX + 50),
            (int)(hpBarY + (hpTexH * hpScaleY) + 5),
            (int)(hpBarX + 650), // 충분히 큰 너비
            (int)(hpBarY + (hpTexH * hpScaleY) + 30)
        };

        m_smallFont->DrawTextA(
            g_sprite,
            hpStr,
            -1,
            &hpTextRc,
            DT_LEFT | DT_NOCLIP,
            D3DCOLOR_ARGB(255, 255, 255, 255)
        );

        g_sprite->End();
    }

    // UI_hart 배경 그리기 (RenderDraw 내부 Begin/End 처리)
    float hartX = 15.f;
    float hartY = -10.f;
    float hartscale = 0.5f;

    m_hartBackSprite.SetColor(255, 255, 255, 255);
    m_hartBackSprite.RenderDraw(
        hartX, hartY,
        0.f, 0.f,
        (float)m_hartBackSprite.imagesinfo.Width,
        (float)m_hartBackSprite.imagesinfo.Height,
        0.f,
        hartscale, hartscale
    );

    // 오른쪽 위 UI
    {
        float scale = 0.65f;
        float w = (float)m_rightTopSprite.imagesinfo.Width;
        float h = (float)m_rightTopSprite.imagesinfo.Height;
        float x = SCREEN_RIGHT - (w * scale);
        float y = 0.f;

        m_rightTopSprite.SetColor(255, 255, 255, 255);
        m_rightTopSprite.RenderDraw(x, y, 0.f, 0.f, w, h, 0.f, scale, scale);
    }

    // 오른쪽 아래 UI
    {
        float scale = 0.65f;
        float w = (float)m_rightBotSprite.imagesinfo.Width;
        float h = (float)m_rightBotSprite.imagesinfo.Height;
        float x = SCREEN_RIGHT - (w * scale);
        float y = SCREEN_BOTTOM - (h * scale);

        m_rightBotSprite.SetColor(255, 255, 255, 255);
        m_rightBotSprite.RenderDraw(x, y, 0.f, 0.f, w, h, 0.f, scale, scale);
    }

    // --- 골드 UI 영역 ---

    // 골드 UI 배경
    float goldBackW = (float)m_goldBackSprite.imagesinfo.Width;
    float goldBackH = (float)m_goldBackSprite.imagesinfo.Height;
    float scale = 0.4f;

    m_goldBackSprite.SetColor(255, 255, 255, 255);
    m_goldBackSprite.RenderDraw(
        0.f, 0.f,
        0.f, 0.f,
        goldBackW, goldBackH,
        0.f,
        scale, scale
    );

    // money_bag.png 스프라이트 처리
    int frameIndex = 0;
    if (m_gold >= 1000) frameIndex = 5;
    else if (m_gold >= 500) frameIndex = 4;
    else if (m_gold >= 200) frameIndex = 3;
    else if (m_gold >= 100) frameIndex = 2;
    else if (m_gold >= 50) frameIndex = 1;
    else frameIndex = 0;

    // 위치 및 스케일
    float bagScale = 0.3f;
    float bagX = 28.f;
    float bagY = 17.f;

    // 실제 텍스처 크기 사용
    float texW = (float)m_goldFrameSprites[frameIndex].imagesinfo.Width;
    float texH = (float)m_goldFrameSprites[frameIndex].imagesinfo.Height;
    float sx = 0.f;
    float sy = 0.f;
    float sw = 140; // 70.f
    float sh = 140; // 70.f

    // 혹시 이미지가 깨지거나 작게 보이면 디버그 출력도 해보자
    // char dbg[100]; sprintf(dbg, "W: %.1f, H: %.1f\n", texW, texH); OutputDebugStringA(dbg);

   
    m_goldFrameSprites[frameIndex].SetColor(255, 255, 255, 255);
    m_goldFrameSprites[frameIndex].RenderDraw(
        bagX, bagY,
        sx, sy,
        sw, sh,
        0.f,
        bagScale, bagScale
    );

    // 골드 텍스트 출력 위치 조정
    float textX = bagX + 5;
    float textY = bagY + (73.f * bagScale) + 25.5f;

    char goldStr[32];
    sprintf_s(goldStr, " %d  ", m_gold);

    if (m_smallFont && g_sprite)
    {
        g_sprite->Begin(D3DXSPRITE_ALPHABLEND);

        RECT textRc = {
            (int)textX - 2,
            (int)textY,
            (int)(textX + 500),
            (int)(textY + 20)
        };

        m_smallFont->DrawTextA(
            g_sprite,
            goldStr,
            -1,
            &textRc,
            DT_LEFT | DT_NOCLIP,
            D3DCOLOR_ARGB(255, 255, 255, 255)
        );

        g_sprite->End();
    }
 

    // 인벤토리 열렸으면 그리기 (Begin/End 분리 필요)
    if (m_inventoryOpen)
    {
        if (g_sprite)
        {
          
            DrawDarkOverlay();  // 내부에 Begin/End 없음
            DrawInventory();    // 내부에 Begin/End 없음

        }
        DebugDrawInventorySlots();
    }
}

//인벤토리 함수
void UIManager::DrawInventory()
{
    /*OutputDebugStringA("[UIManager] DrawInventory 아이템 슬롯 그리기 시작\n");*/

    if (!m_inventoryOpen || !m_inventoryTex) return;

    float invScale = 0.5f;
    float screenCenterX = 700.0f;
    float screenCenterY = 400.0f;

    float invWidth = (float)m_inventorySprite.imagesinfo.Width;
    float invHeight = (float)m_inventorySprite.imagesinfo.Height;

    float posX = screenCenterX - (invWidth * invScale) / 2.0f;
    float posY = screenCenterY - (invHeight * invScale) / 2.0f;

    // 1) 인벤토리 배경 그리기
    m_inventorySprite.SetColor(255, 255, 255, 255);
    m_inventorySprite.RenderDraw(
        posX, posY,
        0.f, 0.f,
        invWidth, invHeight,
        0.f,
        invScale, invScale
    );

    // 2) 슬롯, 아이템 그리기
    for (int i = 0; i < 20; ++i)
    {
        const RECT& slotRect = m_inventorySlots[i];

        if (m_slots[i].type == DropType::NONE)
            continue;

        LPDIRECT3DTEXTURE9 itemTex = nullptr;
        switch (m_slots[i].type)
        {
        case DropType::HP_POTION:
            itemTex = m_potionTex;
            break;
            // TODO: 다른 아이템 타입 추가 가능
        default:
            itemTex = nullptr;
            break;
        }

        if (itemTex)
        {
            Sprite itemSprite;
            itemSprite.SetTexture(itemTex);
            itemSprite.SetColor(255, 255, 255, 255);

            // 슬롯 크기
            float slotWidth = (float)(slotRect.right - slotRect.left);
            float slotHeight = (float)(slotRect.bottom - slotRect.top);

            float texW = 26.f; // 포션 원본 크기 (가로)
            float texH = 30.f; // 포션 원본 크기 (세로)

            float scaleX = slotWidth / texW;
            float scaleY = slotHeight / texH;
            float invScale = std::min(scaleX, scaleY);

            invScale *= 0.6f; // 포션 크기 축소

            // 중앙 정렬 + 오프셋 보정
            float centerX = slotRect.left + (slotWidth - texW * invScale) / 2.0f + 15.0f;
            float centerY = slotRect.top + (slotHeight - texH * invScale) / 2.0f + 12.0f;

            itemSprite.RenderDraw(centerX, centerY, 0.f, 0.f, texW, texH, 0.f, invScale, invScale);
        }

        // 수량 표시
        if (m_smallFont && m_slots[i].quantity > 0)  // 0보다 클 때 모두 표시
        {
            char qtyText[16];

            if (m_slots[i].quantity == 1)
                sprintf_s(qtyText, "1");           // 1개일 땐 그냥 1
            else
                sprintf_s(qtyText, "x%d", m_slots[i].quantity);  // 2개 이상은 x숫자

            RECT qtyRect = {
                slotRect.right - 30,
                slotRect.bottom - 22,
                slotRect.right + 5,
                slotRect.bottom - 2
            };

            g_sprite->Begin(D3DXSPRITE_ALPHABLEND);
            m_smallFont->DrawTextA(g_sprite, qtyText, -1, &qtyRect, DT_RIGHT | DT_BOTTOM, D3DCOLOR_ARGB(255, 255, 255, 255));
            g_sprite->End();
        }
    }

    // 3) 인벤토리 커서 그리기
    if (m_selectedSlotIndex >= 0 && m_selectedSlotIndex < (int)m_inventorySlots.size())
    {
        const RECT& rc = m_inventorySlots[m_selectedSlotIndex];

        float x = (float)rc.left+10.0f;
        float y = (float)rc.top;

        float w = 136.f;  // ← 이미지 원래 사이즈에 맞게 조절
        float h = 136.f;
        float scale = 0.55f;

        float cropX = 0.f; // ← 왼쪽으로 10픽셀 이동
        float cropY = 0.f;


        m_cursorSprite.SetColor(255, 255, 0, 255); // 노란색 테두리
        m_cursorSprite.RenderDraw(x, y, cropX, cropY, w, h, 0.f, scale, scale);
    }
    // 4) 하단 인벤토리 슬롯 아이템 이름 표시 (조건에 맞게)
    if (m_selectedSlotIndex >= 0 && m_selectedSlotIndex < INVENTORY_SLOT_COUNT)
    {
        const InventorySlot& selectedSlot = m_slots[m_selectedSlotIndex];

        // 아이템이 있을 때만 배경 이미지(item_name) 출력
        if (selectedSlot.type != DropType::NONE && selectedSlot.quantity > 0)
        {
            float itemNamePosX = screenCenterX - (700.f / 2.0f) - 20.f;  // 화면 중앙 기준 위치
            float itemNamePosY = posY + invHeight * invScale + 10.f; // 인벤토리 UI 바로 아래 빈 공간 (적당히 여백)

            // 1) 배경 이미지 그리기 (item_name)
            if (m_itemNameTex)
            {
                Sprite itemNameSprite;
                itemNameSprite.SetTexture(m_itemNameTex);
                itemNameSprite.SetColor(255, 255, 255, 255);
                itemNameSprite.RenderDraw(itemNamePosX, itemNamePosY, 0.f, 0.f, 700.f, 54.f, 0.f, 1.f, 1.f);
            }

            // 2) 선택된 슬롯이 HP 포션일 때만 "HP 포션" 텍스트 이미지 출력
            if (selectedSlot.type == DropType::HP_POTION && m_nameHpPotionTex)
            {
                // 텍스트 이미지는 배경 이미지 위에 겹치도록 좌표 맞추기 (적절한 위치로 조정)
                float textPosX = itemNamePosX + 200.f;  // 배경 왼쪽에서 20px 떨어진 위치 (조절 가능)
                float textPosY = itemNamePosY + (54.f - 24.f) / 2.0f; // 배경 높이 대비 중앙 정렬

                Sprite textSprite;
                textSprite.SetTexture(m_nameHpPotionTex);
                textSprite.SetColor(255, 255, 255, 255);
                textSprite.RenderDraw(textPosX, textPosY, 0.f, 0.f, 200.f, 40.f, 0.f, 1.f, 1.f);
            }
        }
    }


}

void UIManager::AddItemToInventory(DropType type, int amount)
{
    if(type == DropType::NONE || type == DropType::GOLD)
        return; // GOLD는 인벤토리에 넣지 않음


    // 같은 아이템 타입이 이미 존재하는 슬롯 찾기
    for (int i = 0; i < 20; ++i) {
        if (m_slots[i].type == type) {
            m_slots[i].quantity += amount;

            // 디버깅 출력
            char buf[128];
            sprintf_s(buf, "[인벤토리] 기존 슬롯에 추가: 타입 = %d, 수량 = %d (슬롯 %d)\n", (int)type, m_slots[i].quantity, i);
            OutputDebugStringA(buf);
            return;
        }
    }

    // 비어있는 슬롯 찾기
    for (int i = 0; i < 20; ++i) {
        if (m_slots[i].type == DropType::NONE) {
            m_slots[i].type = type;
            m_slots[i].quantity = amount;

            // 디버깅 출력
            char buf[128];
            sprintf_s(buf, "[인벤토리] 새 슬롯 추가: 타입 = %d, 수량 = %d (슬롯 %d)\n", (int)type, amount, i);
            OutputDebugStringA(buf);
            return;
        }
    }
    // 슬롯 다 찼을 경우 (선택사항: 경고 출력)
    OutputDebugStringA("인벤토리가 가득 찼습니다!\n");
}


//인벤토리 토글 함수
void UIManager::ToggleInventory()
{
   /* OutputDebugStringA(m_inventoryOpen ? "인벤토리 열림\n" : "인벤토리 닫힘\n");*/
    m_inventoryOpen = !m_inventoryOpen;
}
bool UIManager::IsInventoryOpen() const
{
    return m_inventoryOpen;
}


void UIManager::DrawDarkOverlay()
{
    if (!g_sprite || !m_device) return;

    if (m_blackTex == nullptr)
    {
        HRESULT hr = m_device->CreateTexture(1, 1, 1, 0,
            D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
            &m_blackTex, NULL);
        if (FAILED(hr)) {
            return;
        }

        D3DLOCKED_RECT d3dRect;
        m_blackTex->LockRect(0, &d3dRect, NULL, 0);
        *((DWORD*)d3dRect.pBits) = 0xFF000000; // 완전 검은색 (ARGB)
        m_blackTex->UnlockRect(0);

        m_blackSprite.Texture = m_blackTex;
        m_blackSprite.imagesinfo.Width = 1;
        m_blackSprite.imagesinfo.Height = 1;
        m_blackSprite.SetColor(0, 0, 0, 200);  // 알파 150, 검은색
    }

    // 화면 전체 덮기 (1600x900 기준)
    m_blackSprite.RenderDraw(
        0.f, 0.f,        // 화면 좌표 왼쪽 위
        0.f, 0.f,        // 원본 텍스처 좌표 (1x1)
        1.f, 1.f,        // 원본 텍스처 크기
        0.f,             // 회전 없음
        1600.f, 900.f    // 스케일로 화면 전체 크기 맞춤
    );
}

void UIManager::DebugDrawInventorySlots()
{
    if (!g_showColliderBox) return;
    for (const RECT& slot : m_inventorySlots)
    {
        D3DCOLOR color = D3DCOLOR_ARGB(255, 0, 255, 0);  // 연두색
        Collider collider;
        collider.BoxSow(slot, 0, 0, color);
    }
}

void UIManager::InitInventorySlots()
{
    m_inventorySlots.clear();

    const int SLOT_WIDTH = 66.5;
    const int SLOT_HEIGHT = 66.5;
    const int COLUMNS = 5;
    const int ROWS = 4;

    const int startX = 260;  // 왼쪽으로 3px 이동
    const int startY = 190;  // 아래로 8px 이동


    const int paddingX = 5;  // 가로 간격
    const int paddingY = 6;  // 세로 간격 (줄 간격)

    // 콜라이더 축소량 및 이동 오프셋 (원하는 값으로 조절 가능)
    const int shrinkAmountX = 8;  // 좌우 줄일 픽셀 수
    const int shrinkAmountY = 15; // 상하 줄일 픽셀 수
    const int offsetX = 2;        // 좌측으로 이동할 픽셀 수 (음수면 우측)
    const int offsetY = 12;        // 위로 이동할 픽셀 수 (음수면 아래)
    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLUMNS; ++col)
        {
            RECT rc;
            int currentOffsetY = offsetY;

            if (row == 2)
            {
                currentOffsetY -= 10;  // 3번째 줄 위로 10픽셀
            }
            else if (row == 3)
            {
                currentOffsetY -= 18;  // 4번째 줄은 더 위로 15픽셀 (필요하면 값 조절)
            }

            rc.left = startX + col * (SLOT_WIDTH + paddingX) + offsetX;
            rc.top = startY + row * (SLOT_HEIGHT + paddingY) + currentOffsetY;
            rc.right = rc.left + SLOT_WIDTH - shrinkAmountX;
            rc.bottom = rc.top + SLOT_HEIGHT - shrinkAmountY;
            m_inventorySlots.push_back(rc);
        }
    }
}

void UIManager::UpdateInventoryCursor()
{
    if (!m_inventoryOpen) return;

    bool moved = false;

    bool curW = (GetAsyncKeyState('W') & 0x8000) != 0;
    bool curA = (GetAsyncKeyState('A') & 0x8000) != 0;
    bool curS = (GetAsyncKeyState('S') & 0x8000) != 0;
    bool curD = (GetAsyncKeyState('D') & 0x8000) != 0;

    if (curW && !m_prevW && m_cursorY > 0) { m_cursorY--; moved = true; }
    if (curS && !m_prevS && m_cursorY < 3) { m_cursorY++; moved = true; }
    if (curA && !m_prevA && m_cursorX > 0) { m_cursorX--; moved = true; }
    if (curD && !m_prevD && m_cursorX < 4) { m_cursorX++; moved = true; }

    // 이전 입력 상태 저장
    m_prevW = curW;
    m_prevA = curA;
    m_prevS = curS;
    m_prevD = curD;

    // 실제 인벤토리 인덱스 갱신
    if (moved)
        m_selectedSlotIndex = m_cursorY * 5 + m_cursorX;
}


D3DXVECTOR2 UIManager::GetInventorySlotPosition(DropType type) {
    // 예: 오른쪽 아래 인벤토리 슬롯 좌표 하드코딩 또는 계산
    switch (type) {
    case DropType::GOLD:
        return D3DXVECTOR2(100.f, 50.f);
    case DropType::HP_POTION:
        return D3DXVECTOR2(1460.f, 200.f);
    default:
        return D3DXVECTOR2(1400.f, 700.f);
    }
}

void UIManager::UseSelectedItem(Hero& hero)
{
    int index = m_cursorY * m_inventoryCols + m_cursorX;

    if (index < 0 || index >= INVENTORY_SLOT_COUNT)
        return;

    InventorySlot& slot = m_slots[index];

    if (slot.type == DropType::HP_POTION && slot.quantity > 0)
    {
        hero.Heal(50);  

        slot.quantity--;
        if (slot.quantity <= 0)
        {
            slot.type = DropType::NONE;
            slot.quantity = 0;
        }
    }
}

void UIManager::DrawBossHpBar(float currentHp, float maxHp, int x, int y)
{
    /*OutputDebugStringA("[BossHP] DrawBossHpBar 호출됨\n");*/

    if (!m_device || !m_bossHpBarBackSprite.Texture || !m_bossHpBarFrontSprite.Texture)
        return;

    float hpRatio = maxHp > 0 ? currentHp / maxHp : 0.0f;
    hpRatio = max(0.0f, min(hpRatio, 1.0f));

    RECT rcBack = { 0, 0, m_bossHpBarBackSprite.imagesinfo.Width, m_bossHpBarBackSprite.imagesinfo.Height };
    RECT rcFront = { 0, 0, (LONG)(m_bossHpBarFrontSprite.imagesinfo.Width * hpRatio), m_bossHpBarFrontSprite.imagesinfo.Height };

    D3DXVECTOR3 posBack((float)x, (float)y, 0);
    D3DXVECTOR3 posFront((float)x, (float)y, 0);

    g_sprite->Begin(D3DXSPRITE_ALPHABLEND);

    g_sprite->Draw(m_bossHpBarBackSprite.Texture, &rcBack, nullptr, &posBack, D3DCOLOR_XRGB(255, 255, 255));
    g_sprite->Draw(m_bossHpBarFrontSprite.Texture, &rcFront, nullptr, &posFront, D3DCOLOR_XRGB(255, 255, 255));

    g_sprite->End();
}