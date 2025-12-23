#pragma once
#include "Include.h"

// 문 상태 열거형
enum class DoorState {
    CLOSED,     // 닫힘
    OPENING,    // 열리는 중
    OPENED      // 열린 상태 (멈춤)
};


// 도어 스프라이트 관련 상수
constexpr float DOOR_FRAME_W = 174;  // ≈ 171.83
constexpr float DOOR_FRAME_H = 96;   // = 96
constexpr int DOOR_FRAME_COUNT = 6;
constexpr int DOOR_FRAME_DELAY = 100;


class Door {
public:

    int GetFrame() const { return m_frame; }  // 디버깅용
    // 초기화 (이미지 경로, 위쪽 문인지 여부)
    void Init(const char* filename, bool isTopDoor);

    // 매 프레임 호출: 애니메이션 처리
    void Update();

    // 화면에 그리기
    void Draw();

    // 외부 조건으로 문 열기 트리거
    void TriggerOpen();

    // 위치 설정 (x, y)
    void SetPos(float x, float y);

    void TriggerClose();

    float GetPosX() const { return m_x; }
    float GetPosY() const { return m_y; }

    void SetBossDoorTexture(LPDIRECT3DTEXTURE9 tex) {
        m_bossDoorTex = tex;
        m_isBossDoor = (tex != nullptr);
    }

    // 플레이어와 충돌 판정용 트리거 영역 반환
    RECT GetTriggerRc() const;

    // 현재 상태 반환 (사용자용)
    DoorState GetState() const { return m_state; }

    // 보스 도어인지 여부 반환
    bool IsBossDoor() const { return m_isBossDoor; }


private:

   

    Sprite m_sprite;          // 문 이미지 스프라이트
    DoorState m_state = DoorState::CLOSED;  // 현재 상태

    float m_x = 0.f, m_y = 0.f;   // 위치
    int m_frame = 0;              // 현재 프레임
    ULONGLONG m_animTime = 0;     // 마지막 애니메이션 시간

    bool m_isTop = true;         // true: 위쪽 문, false: 아래쪽 문
    RECT m_triggerRc = {};       // 문 앞 충돌 감지 영역

    LPDIRECT3DTEXTURE9 m_bossDoorTex = nullptr;
    bool m_isBossDoor = false;
};