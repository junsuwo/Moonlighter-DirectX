#include "Door.h"

extern LPD3DXSPRITE g_sprite;
// 도어 초기화
void Door::Init(const char* filename, bool isTopDoor) {
    // 도어 스프라이트 이미지 로드
    m_sprite.Create(filename, false, D3DCOLOR_XRGB(255, 0, 255));  // 마젠타(보통 투명색)

    m_isTop = isTopDoor;       // 위쪽 문인지 여부 저장
    m_state = DoorState::CLOSED;  // 시작 상태는 닫힘
    m_frame = 0;
    m_animTime = 0;
}

// 도어 위치 설정 및 충돌 영역 계산
void Door::SetPos(float x, float y) {
    m_x = x;
    m_y = y;

    // 충돌 영역(트리거 박스) 설정 - 문 중앙 기준으로 약간 작게 (수정됨)
    const int COLLISION_MARGIN_X = 45;      // 좌우 오프셋 축소 (기존 40 → 30)
    const int COLLISION_MARGIN_Y_TOP = 20;  // 위쪽 오프셋 유지
    const int COLLISION_MARGIN_Y_BOTTOM = 10; // 아래쪽 오프셋 유지

    int boxOffsetX = -18;

    m_triggerRc.left = static_cast<LONG>(m_x + COLLISION_MARGIN_X + boxOffsetX);
    m_triggerRc.right = static_cast<LONG>(m_x + DOOR_FRAME_W - COLLISION_MARGIN_X + boxOffsetX);
    m_triggerRc.top = static_cast<LONG>(m_y + COLLISION_MARGIN_Y_TOP);
    m_triggerRc.bottom = static_cast<LONG>(m_y + DOOR_FRAME_H - COLLISION_MARGIN_Y_BOTTOM);
}
// 플레이어 충돌 체크용 트리거 사각형 반환
RECT Door::GetTriggerRc() const {
    return m_triggerRc;
}

// 외부 이벤트에 의해 문 열기 트리거
void Door::TriggerOpen() {
    if (m_state == DoorState::CLOSED) {
       
        m_state = DoorState::OPENING;
        m_animTime = GetTickCount64();
        m_frame = 0;
    }
}

// 애니메이션 진행
void Door::Update() {
    if (m_state == DoorState::OPENING) {
        ULONGLONG now = GetTickCount64();
        if (now - m_animTime >= DOOR_FRAME_DELAY) {
            m_frame++;
            m_animTime = now;
            if (m_frame >= DOOR_FRAME_COUNT) {
                m_frame = DOOR_FRAME_COUNT - 1;
                m_state = DoorState::OPENED;
            }
        }
    }
}

// 도어 이미지 그리기
void Door::Draw() {
    // 보스 도어 윗부분 위치 조절용 오프셋 (문 위치에 상대적인 픽셀값)
    const float BOSS_DOOR_OFFSET_X = -10.f;  // 필요시 조절
    const float BOSS_DOOR_OFFSET_Y = 0.f;

    int row = m_isTop ? 0 : 1;
    int frameIndex = DOOR_FRAME_COUNT - 1 - m_frame;

    int srcLeft = frameIndex * DOOR_FRAME_W;
    int srcTop = row * DOOR_FRAME_H;
    int srcRight = srcLeft + DOOR_FRAME_W;
    int srcBottom = srcTop + DOOR_FRAME_H;

    // 닫힌 상태(frameIndex = 5)를 기준으로 → 작아질수록 오른쪽으로 보정
    float shiftX = (5 - frameIndex) * 1.0f;
    float drawX = m_x + shiftX;
    float drawY = m_y;

    // === 일반 문 애니메이션 ===
    m_sprite.RenderDraw(
        drawX, drawY,
        (float)srcLeft, (float)srcTop,
        (float)srcRight, (float)srcBottom,
        0.0f,
        1.0f, 1.0f
    );

    // === 보스 도어 윗부분 덮어씌우기 ===
    if (m_isBossDoor && m_bossDoorTex) {
        RECT srcRect = { 0, 0, 120, 64 };  // 실제 보스 도어 이미지 크기

        // 목표 사이즈: DOOR_FRAME_W x 64로 맞추고 싶다고 가정
        float targetW = DOOR_FRAME_W;
        float targetH = 64.f;

        float scaleX = targetW / 120.f;
        float scaleY = targetH / 64.f;

        float bossDrawX = drawX + BOSS_DOOR_OFFSET_X;
        float bossDrawY = drawY + BOSS_DOOR_OFFSET_Y;

        D3DXMATRIX matOld, matNew;
        g_sprite->GetTransform(&matOld);

        // 스케일 적용 매트릭스 (보스 도어 위치를 기준으로 확대)
        D3DXMatrixTransformation2D(
            &matNew,
            &D3DXVECTOR2(bossDrawX, bossDrawY), // 스케일 기준점
            0.f,
            &D3DXVECTOR2(scaleX, scaleY),       // 스케일 비율
            nullptr,
            0.f,
            nullptr
        );

        g_sprite->SetTransform(&matNew);

        g_sprite->Begin(D3DXSPRITE_ALPHABLEND);
        g_sprite->Draw(m_bossDoorTex, &srcRect, nullptr, &D3DXVECTOR3(bossDrawX, bossDrawY, 0), D3DCOLOR_ARGB(255, 255, 255, 255));
        g_sprite->End();

        g_sprite->SetTransform(&matOld);  // 원래대로 되돌리기
    }
}
void Door::TriggerClose()
{
    if (m_state != DoorState::CLOSED) {
        m_state = DoorState::CLOSED;
        m_frame = 0;          // 닫힌 상태 프레임으로 초기화
        m_animTime = GetTickCount64();
    }
}