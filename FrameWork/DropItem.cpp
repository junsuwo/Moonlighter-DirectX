#include "Collider.h"
#include "DropItem.h"


extern Collider coll;

DropItem::DropItem(DropType type, float x, float y, LPDIRECT3DTEXTURE9 texture, float groundY)
    : m_type(type), m_pos(x, y), m_texture(texture), m_velocity(0, 0), m_collected(false), m_onGround(false), m_groundY(groundY)
{
    // 충돌 박스 초기 위치 및 크기 설정 (16픽셀 반경 사각형 예시)
    m_rc.left = static_cast<LONG>(m_pos.x - 12);
    m_rc.top = static_cast<LONG>(m_pos.y - 12);
    m_rc.right = static_cast<LONG>(m_pos.x + 12);
    m_rc.bottom = static_cast<LONG>(m_pos.y + 12);

    m_initialY = y; // 최초 위치 저장
}

DropItem::~DropItem()
{
    // 텍스처는 외부에서 관리하므로 해제하지 않음
}

void DropItem::Update(float dt)
{
    if (m_collected && m_isFlyingToUI)
    {
        m_flyTime += dt;
        float t = min(m_flyTime / m_flyDuration, 1.5f);

        float startX = m_flyStartPos.x;
        float startY = m_flyStartPos.y;
        float targetX = m_flyTargetPos.x;
        float targetY = m_flyTargetPos.y;

        float height = 100.0f;  // 포물선 최대 높이 조절 가능

        // 포물선 위치 계산 (t는 0~1 사이)
        float newX = startX + (targetX - startX) * t;
        float newY = startY + (targetY - startY) * t - height * 4 * t * (1 - t);

        m_pos = D3DXVECTOR2(newX, newY);

        if (t >= 1.0f)
        {
            m_isFlyingToUI = false; // 도착 완료
        }

        UpdateCollisionBox();
        return;
    }

    // 이하 기존 중력, 바닥 애니메이션 처리
    const float gravity = 500.0f;

    if (!m_onGround)
    {
        m_velocity.y += gravity * dt;
        m_pos += m_velocity * dt;

        if (m_pos.y >= m_groundY)
        {
            m_pos.y = m_groundY;
            m_velocity.y = 0;
            m_onGround = true;
            m_initialY = m_groundY;
        }
    }
    else
    {
        m_elapsed += dt;
        m_pos.y = m_initialY + sin(m_elapsed * 3.0f) * 5.0f;
    }

    UpdateCollisionBox();
}

void DropItem::Render(LPD3DXSPRITE sprite)
{
    // 날아가고 있을 땐 m_collected가 true여도 그려야 하므로 조건 수정
    if (m_collected && !m_isFlyingToUI) return;


    if (m_texture && sprite)
    {
        // 스케일 지정 (예: 0.7배 크기)
        D3DXMATRIX matScale;
        D3DXMatrixScaling(&matScale, 0.8f, 0.8f, 0.8f);
        sprite->SetTransform(&matScale);

        RECT srcRect;
        switch (m_type)
        {
        case DropType::GOLD:
            srcRect = { 0, 0, 30, 30 };       // gold01.png 크기
            break;
        case DropType::HP_POTION:
            srcRect = { 0, 0, 29, 34 };       // potion_B.png 크기
            break;
        default:
            srcRect = { 0, 0, 32, 32 };       // 기본값
            break;
        }

        D3DXVECTOR3 pos3(m_pos.x, m_pos.y, 0);

        sprite->Begin(D3DXSPRITE_ALPHABLEND);
        sprite->Draw(m_texture, &srcRect, nullptr, &pos3, D3DCOLOR_XRGB(255, 255, 255));
        sprite->End();
        //coll.BoxSow(m_rc, 0, 0, D3DCOLOR_ARGB(255, 0, 255, 0));  // 초록색
        // 스케일 초기화 (기본값)로 돌려놓기
        D3DXMatrixScaling(&matScale, 1.0f, 1.0f, 1.0f);
        sprite->SetTransform(&matScale);

       
    }
}

bool DropItem::CheckCollisionWithHero(const RECT& heroRc)
{
    if (m_collected) return false; // 이미 수집된 아이템 무시

    // 히어로 충돌 박스 확장 (예: 좌우 위아래 20픽셀씩)
    RECT expandedHeroRc = {
         heroRc.left - 30,
    heroRc.top - 30,
    heroRc.right + 30,
    heroRc.bottom + 30
    };

    RECT intersect;
    bool collide = IntersectRect(&intersect, &m_rc, &expandedHeroRc);

    // 디버그 출력: 아이템 위치, 히어로 박스, 충돌 여부 출력
   /* char buf[256];
    sprintf(buf, "DropItem 위치: (%.1f, %.1f), 히어로 RC: (%d, %d, %d, %d), 충돌: %d\n",
        m_pos.x, m_pos.y,
        heroRc.left, heroRc.top, heroRc.right, heroRc.bottom,
        collide);
    OutputDebugStringA(buf);*/

    if (collide)
    {
        m_collected = true;
        return true;
    }
    return false;
}


void DropItem::UpdateCollisionBox()
{
    //// 아이템 기준 충돌 박스 크기, 위치 조정
    //int width = 30;
    //int height = 30;
    //int offset = 10; // 아이템 충돌 박스를 아이템 위치 중심에서 살짝 넓게

    //m_rc.left = static_cast<LONG>(m_pos.x) - offset;
    //m_rc.top = static_cast<LONG>(m_pos.y) - offset;
    //m_rc.right = m_rc.left + width + offset * 2;
    //m_rc.bottom = m_rc.top + height + offset * 2;

    int width = 40;
    int height = 30;

    // m_pos는 아이템 바닥 위치라 가정
    // 충돌박스를 m_pos 기준으로 좌측 상단을 조금 위, 왼쪽으로 당겨줌
    int offsetX = 100;  // width/2 기본값인데, 더 키우거나 줄일 수 있음
    int offsetY = 40;  // height보다 조금 크게 잡아 아이템 위쪽까지 포함

    m_rc.left = static_cast<LONG>(m_pos.x - offsetX);
    m_rc.right = m_rc.left + width;
    m_rc.bottom = static_cast<LONG>(m_pos.y - 80);  // 바닥에서 살짝 위로 올림 (5 픽셀)
    m_rc.top = m_rc.bottom - height;
}

// 날라가기
void DropItem::StartFlyToUI(const D3DXVECTOR2& targetPos) {
    m_isFlyingToUI = true;
    m_flyStartPos = m_pos;
    m_flyTargetPos = targetPos;
    m_flyTime = 0.0f;
}