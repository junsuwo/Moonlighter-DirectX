#include "Global.h"
#include "DamageTextManager.h"
#include <string>


extern LPDIRECT3DDEVICE9 g_device;

void DamageTextManager::Init()
{
    HRESULT hr = D3DXCreateFontA(g_pd3dDevice, 24, 0, FW_BOLD, 1, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        "Arial", &m_font);
    if (FAILED(hr)) {
        OutputDebugStringA("DamageTextManager 폰트 생성 실패\n");
    }
}

void DamageTextManager::Update(float deltaTime)
{
    for (auto it = m_texts.begin(); it != m_texts.end(); )
    {
        it->lifetime += deltaTime;
        it->position.y -= it->riseSpeed * deltaTime; // 위로 떠오름

        if (it->lifetime > it->maxLifetime)
            it = m_texts.erase(it);
        else
            ++it;
    }
}

void DamageTextManager::Render()
{
    if (!m_font) return;

    for (const auto& text : m_texts)
    {
        WCHAR buffer[32];
        swprintf_s(buffer, L"-%d", text.damage);

        RECT rc;
        int offsetX = +20; // 오른쪽으로 이동시키고 싶으면 음수를 줄이세요
        int offsetY = +50; // 아래로 내리고 싶으면 음수를 줄이세요
        rc.left = static_cast<LONG>(text.position.x) + offsetX;
        rc.top = static_cast<LONG>(text.position.y) + offsetY;
        rc.right = rc.left + 100;  // 텍스트 박스 넓이
        rc.bottom = rc.top + 50;    // 텍스트 박스 높이

        int alpha = static_cast<int>(255 * (1.0f - text.lifetime / text.maxLifetime));
        D3DCOLOR fadeColor = (text.color & 0x00FFFFFF) | (alpha << 24);

        m_font->DrawTextW(NULL, buffer, -1, &rc,
            DT_CENTER | DT_VCENTER | DT_NOCLIP,
            fadeColor);
    }
}

void DamageTextManager::AddText(D3DXVECTOR2 position, int damage, D3DCOLOR color)
{
    float shorterLife = 0.2f;
    m_texts.emplace_back(position, damage, shorterLife);
    m_texts.back().color = color;
}

void DamageTextManager::Release()
{
    if (m_font)
    {
        m_font->Release();
        m_font = nullptr;
    }
    m_texts.clear();
}