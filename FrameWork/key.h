#pragma once
#include "Include.h"

/*  ──────────────────────────────────────────────
    Key  클래스
      · Update()   : 매 프레임 키 갱신
      · Down()     : 키가 계속 눌려 있는가
      · Pressed()  : 이번 프레임에 막 눌림 (Edge ↑→↓)
      · Released() : 이번 프레임에 막 뗌  (Edge ↓→↑)
    ────────────────────────────────────────────── */
class Key
{
public:
    Key();                     // 기본 생성자
    ~Key() = default;          // 기본 소멸자

    void Update();             // 매 프레임 호출

    // ── 헬퍼 ───────────────────────────────
    bool Down(int vk)     const;   // 계속 눌림
    bool Pressed(int vk)  const;   // 이번 프레임에 막 눌림
    bool Released(int vk) const;   // 이번 프레임에 막 뗌
    bool KeyUp(int vk) const;
    bool Pressing(int vk) const {
        return (GetAsyncKeyState(vk) & 0x8000) != 0;
    }
    // ──────────────────────────────────────

    // 타이머(디버그용 단축키 등에 사용)
    DWORD KeyTime  = 0;   // 전체 공용
    DWORD KeyTime1 = 0;   // ← →
    DWORD KeyTime2 = 0;   // 스페이스 등
    DWORD KeyTime3 = 0;   // ↑ ↓

private:
    BYTE m_cur [256] = { 0 };   // 이번 프레임 키 상태
    BYTE m_prev[256] = { 0 };   // 이전 프레임 키 상태
};

extern Key key;   // 전역 키 객체