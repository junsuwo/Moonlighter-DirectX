#pragma once
#include <d3dx9.h>
#include <string>

class DamageText
{
public:
    DamageText(float x, float y, int damage);

    void Update(float deltaTime);
    void Draw();

    bool IsFinished() const { return m_finished; }

private:
    D3DXVECTOR2 m_position;
    std::string m_text;
    float m_lifeTime;        // 얼마 동안 유지될지
    float m_alpha;           // 투명도 (점점 사라지게)
    float m_riseSpeed;       // 위로 올라가는 속도
    bool m_finished;
};