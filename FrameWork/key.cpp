#include "Include.h"
#include "UIManager.h"

extern UIManager g_UIManager;
extern Hero hero;
using Dir = Hero::Direction;

Key key;

/* ── 생성자 ───────────────────────────────── */
Key::Key()
{
    // 배열·타이머 클리어
    memset(m_cur, 0, sizeof(m_cur));
    memset(m_prev, 0, sizeof(m_prev));
}

// 동시 키를 위하여 KeyTime, KeyTime1, KeyTime2, KeyTime3 따로 처리

void Key::Update()
{
    memcpy(m_prev, m_cur, sizeof(m_cur));  // 이전 키 상태 저장
    // 1) 모든 키 상태 갱신
    for (int vk = 0; vk < 256; ++vk)
        m_cur[vk] = (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0;

    // 🔑 I키 토글 처리 (항상 처리 가능하도록 먼저 처리)
    if (m_cur['I'] && !m_prev['I'])
    {
        g_UIManager.ToggleInventory();
    }

    // 💡 인벤토리 열려있으면 이동 등 다른 입력은 무시
    if (g_UIManager.IsInventoryOpen())
    {
        // I키 외 입력 무시하므로 조기 리턴
       
        return;
    }

    if (hero.IsDead()) {
        
        return;
    }

    const float SPEED = 5.0f;




    if (hero.IsRolling()) {
        // 방향 변경, 이동 금지

        // 애니메이션은 강제로 롤 애니메이션 유지하도록 처리
        hero.SetAnim(Hero::Anim::ROLL);

        // 키 상태만 저장하고 함수 종료
        
        return;
    }
    else
    {

        // 3) 방향키 상태 체크 (WASD)
        bool left = m_cur['A'];
        bool right = m_cur['D'];
        bool up = m_cur['W'];
        bool down = m_cur['S'];

        float dx = 0.f, dy = 0.f;

        // 4) 대각선 입력 감지 후 한쪽 방향만 허용 (수평 우선 예시)
        bool horizontal = left || right;
        bool vertical = up || down;

        if (horizontal && vertical) {
            // 대각선 입력일 때 수평 방향만 허용 (원하면 vertical로 변경 가능)
            if (left && !right) {
                dx = -SPEED;
                dy = 0.f;
            }
            else if (right && !left) {
                dx = SPEED;
                dy = 0.f;
            }
            else {
                dx = 0.f;
                dy = 0.f;
            }
        }
        else {
            // 대각선 아닌 경우 기존대로 이동
            if (left)  dx = -SPEED;
            else if (right) dx = SPEED;

            if (up)    dy = -SPEED;
            else if (down)  dy = SPEED;
        }

        // 5) 이동 중인지 여부 판단
        bool moving = (dx != 0.f || dy != 0.f);

        // 6) 현재 히어로 위치
        float heroX = hero.GetX();
        float heroY = hero.GetY();

        // 7) 맵 경계 크기 (화면 크기 기준)
        const float mapWidth = 1280.f;
        const float mapHeight = 768.f;

        // 8) 이동 후 예상 위치 계산
        float newX = heroX + dx;
        float newY = heroY + dy;

        // 9) 화면 경계 충돌 처리
        if (newX < 0) newX = 0;
        else if (newX + FRAME_W > mapWidth) newX = mapWidth - FRAME_W;

        if (newY < 0) newY = 0;
        else if (newY + FRAME_H > mapHeight) newY = mapHeight - FRAME_H;

        // 10) 실제 적용할 이동량 계산
        dx = newX - heroX;
        dy = newY - heroY;

        // 11) 방향 설정 (기존 네 방향 우선)
        if (left && !right && !up && !down)
            hero.SetDirection(Hero::Direction::DIR_DOWN);
        else if (right && !left && !up && !down)
            hero.SetDirection(Hero::Direction::DIR_UP);
        else if (up && !left && !right && !down)
            hero.SetDirection(Hero::Direction::DIR_LEFT);
        else if (down && !left && !right && !up)
            hero.SetDirection(Hero::Direction::DIR_RIGHT);

        // 12) 공격 혹은 구르기 중이라면 이동/애니메이션 변경 금지, 방향만 갱신
        if (hero.IsAttacking() || hero.IsRolling()) {
            // 이동 및 애니메이션 변경 금지
            // 단, 방향은 위에서 이미 세팅했으니 그대로 유지
        }
        else {
            // 13) 평소 이동 및 애니메이션 처리
            if (moving)
                hero.SetAnim(Hero::Anim::WALK);
            else
                hero.SetAnim(Hero::Anim::IDLE);

            hero.Move(dx, dy);
        }

    }
}


bool Key::Down(int vk) const
{
    return m_cur[vk];
}

bool Key::Pressed(int vk) const
{
    return !m_prev[vk] && m_cur[vk];
}

bool Key::Released(int vk) const
{
    return m_prev[vk] && !m_cur[vk];
}

bool Key::KeyUp(int vk) const {
    return !m_cur[vk] && m_prev[vk];
}