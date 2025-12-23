#pragma once
#include <d3dx9.h>

class ShakeManager
{
public:
    void Update(float deltaTime);
    void StartShake(float duration, float magnitude);
    D3DXVECTOR2 GetOffset() const;

    bool IsShaking() const { return m_isShaking; }
    static ShakeManager* GetInstance();

private:
    bool m_isShaking = false;
    float m_duration = 0.0f;
    float m_timer = 0.0f;
    float m_magnitude = 0.0f;
    D3DXVECTOR2 m_offset = D3DXVECTOR2(0, 0);
};