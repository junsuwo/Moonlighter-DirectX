#pragma once
#pragma once
#include <vector>
#include <d3dx9.h>

// 데미지 텍스트 하나에 대한 구조
struct DamageText
{
    D3DXVECTOR2 position;
    int damage;
    float lifetime;
    float maxLifetime;
    float riseSpeed;
    D3DCOLOR color;

    DamageText(D3DXVECTOR2 pos, int dmg, float life = 1.0f)
        : position(pos), damage(dmg), lifetime(0.0f), maxLifetime(life),
        riseSpeed(30.0f), color(D3DCOLOR_ARGB(255, 255, 50, 50)) {
    }
};

// 데미지 텍스트 매니저 클래스
class DamageTextManager
{
private:
    std::vector<DamageText> m_texts;
    LPD3DXFONT m_font;

public:
    void Init();
    void Update(float deltaTime);
    void Render();
    void Release();

    void AddText(D3DXVECTOR2 position, int damage, D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 50, 50));
};