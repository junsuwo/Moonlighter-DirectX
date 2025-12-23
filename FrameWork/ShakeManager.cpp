#include "ShakeManager.h"
#include <cstdlib>  // rand

void ShakeManager::Update(float deltaTime)
{
    if (!m_isShaking) {
        m_offset = D3DXVECTOR2(0, 0);
        return;
    }

    m_timer += deltaTime;

    if (m_timer >= m_duration) {
        m_isShaking = false;
        m_offset = D3DXVECTOR2(0, 0);
        return;
    }

    float randX = ((rand() % 2001) / 1000.0f - 1.0f) * m_magnitude;
    float randY = ((rand() % 2001) / 1000.0f - 1.0f) * m_magnitude;

    m_offset = D3DXVECTOR2(randX, randY);
}

void ShakeManager::StartShake(float duration, float magnitude)
{
    m_isShaking = true;
    m_duration = duration;
    m_timer = 0.0f;
    m_magnitude = magnitude;
}

D3DXVECTOR2 ShakeManager::GetOffset() const
{
    return m_offset;
}
ShakeManager* ShakeManager::GetInstance() {
    static ShakeManager instance;
    return &instance;
}