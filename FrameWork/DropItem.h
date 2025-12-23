#ifndef __DROPITEM_H__
#define __DROPITEM_H__

#include <d3dx9.h>

// 드롭 아이템 종류 열거형
enum class DropType {
    NONE,
    GOLD,       // 골드
    HP_POTION   // 체력 회복 물약
};

// 드롭 아이템 클래스
class DropItem
{
private:
    DropType m_type;           // 아이템 종류
    D3DXVECTOR2 m_pos;         // 위치 좌표
    D3DXVECTOR2 m_velocity;    // 이동 속도 (낙하 등)
    RECT m_rc;                 // 충돌 박스
    

    LPDIRECT3DTEXTURE9 m_texture = nullptr; // 아이템 텍스처
    float m_initialY = 0.f;    // 드랍 시 최초 Y 위치 (바닥 고정용)
    bool m_onGround = false;   // 바닥에 닿았는지 여부

    // 날라가기
    bool m_isFlyingToUI = false;        // 인벤토리로 날아가는 중인지
    D3DXVECTOR2 m_flyStartPos;          // 출발 위치
    D3DXVECTOR2 m_flyTargetPos;         // 도착 위치
    float m_flyTime = 0.0f;             // 경과 시간
    float m_flyDuration = 0.3f;         // 날아가는 데 걸리는 시간
    

public:


    // 생성자: 아이템 종류, 위치, 텍스처 주입
    DropItem(DropType type, float x, float y, LPDIRECT3DTEXTURE9 texture, float groundY);
    float m_groundY;
    // 소멸자
    ~DropItem();

    // 매 프레임 업데이트 (위치 갱신 등)
    void Update(float dt);
    bool m_collected = false;  // 수집 여부
    
    float m_elapsed = 0.f;  // 드롭 아이템 개별 애니메이션 타이머
    
// 화면에 렌더링
    void Render(LPD3DXSPRITE sprite);

    // 히어로와 충돌 체크, 충돌 시 수집 처리
    bool CheckCollisionWithHero(const RECT& heroRc);

    void DropItem::UpdateCollisionBox();

    // 수집 여부 반환
    bool IsCollected() const { return m_collected; }

    // 아이템 종류 반환
    DropType GetType() const { return m_type; }

    // 충돌 박스 반환
    const RECT& GetRect() const { return m_rc; }

    void StartFlyToUI(const D3DXVECTOR2& targetPos); // 날기 시작
   
  
    bool IsFlying() const { return m_isFlyingToUI; }

};

#endif