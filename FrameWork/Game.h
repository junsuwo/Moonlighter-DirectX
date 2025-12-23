#ifndef __Game_H__
#define __Game_H__

#include "Door.h"
#include"Map.h"
#include <vector>
#include "DropItem.h"
#include "DamageTextManager.h"


enum class GameState {
    Title,       // 타이틀 화면 (start.png)
    Playing,     // 실제 게임 (Battle, Entrance, Boss map)
    End     // 엔딩 화면 (end.png)
};


class Game : public Chap
{
public:
    Game();
    ~Game();

    DWORD GameTime;
  
    virtual void Init();
    virtual void Update(double frame);
    virtual void Draw();
    virtual void OnMessage(MSG* msg);
    static Game* GetInstance() { return s_instance; }
    bool m_spacePressed = false;

    void ClearDropItems();  // 드롭 아이템 초기화 함수

    // 맵 전환 함수 (슬라이드 방식 제거)
    void ChangeMap(MapType newMap);

    // ===== 🔄 페이드 전환 관련 함수 =====
    void StartFadeTransition(MapType nextMap);   // 페이드 전환 시작
    void UpdateFadeTransition(double frame);     // 알파 업데이트
    bool IsFading() const { return m_isFading; } // 전환 중인지 확인용

    bool m_cameraShaking = false;
    float m_cameraShakeTimer = 0.0f;
    float m_cameraShakeDuration = 0.5f;
    float m_cameraShakeIntensity = 10.0f;


    float m_originalCamScale = 1.0f;
    float m_originalCamPosX = 0.0f;
    float m_originalCamPosY = 0.0f;
    //float m_bossIntroTimer = 0.0f;
    void EnterBossRoom();
    
    
    DamageTextManager& GetDamageTextManager() { return m_damageTextManager; }


private:


    float m_startTextBlinkTimer = 0.0f;
    LPD3DXFONT m_pStartFont;
    Sprite m_titleSprite;  // start.png
    Sprite m_endSprite;    // end.png
    RECT m_startButtonRect;  // 클릭 영역


    DamageTextManager m_damageTextManager;
    // ===== 🗺️ 맵 관련 =====
    MapType m_currentMap = MapType::Entrance;  // 현재 맵
    std::vector<Monster*> m_nextMapMonsters;

    GameState m_gameState = GameState::Title;  // 현재 게임 상태
    float m_endingTimer = 0.0f;                // 엔딩 화면 유지 시간
    LPDIRECT3DTEXTURE9 m_titleTex = nullptr;   // 타이틀 텍스처
    LPDIRECT3DTEXTURE9 m_endTex = nullptr;     // 엔딩 텍스처


  /*  bool m_bossIntroStarted = false;*/
    // ===== 💰 드롭 아이템 관련 =====
    std::vector<DropItem*> dropItems;
    void SpawnDropItems(float x, float y);     // 드롭 아이템 생성 함수
    LPDIRECT3DTEXTURE9 m_potionTex = nullptr;  // 포션 텍스처
    LPDIRECT3DTEXTURE9 m_goldTex = nullptr;    // 골드 텍스처
    std::vector<RECT> m_obstacleRects;  // 맵 장애물 박스 리스트
    // ===== 🚪 문 충돌 무시 타이머 =====
    bool m_ignoreDoorCollision = false;
    ULONGLONG m_ignoreDoorEndTime = 0;
    ID3DXFont* m_pBossNameFont = nullptr;
    // ===== 🌑 페이드 전환 관련 변수 =====
    bool m_isFading = false;                 // 페이드 중인지 여부
    int m_fadeAlpha = 0.0f;                     // 현재 알파 값 (0 ~ 255)
    int m_fadeDirection = 1;                 // 1 = OUT, -1 = IN
    float m_fadeDuration = 0.5f;             // 전환 총 시간 (초)
    float m_fadeElapsed = 0.f;               // 누적 시간
    MapType m_fadeNextMap = MapType::Entrance;   // 전환 완료 후 바꿀 맵
    LPDIRECT3DTEXTURE9 m_blackTex = nullptr;     // 검은색 텍스처
    LPDIRECT3DTEXTURE9 m_bossDoorTex = nullptr;  // 보스 도어 텍스처
    RECT m_bossDoorTriggerRc;  // 보스 도어 충돌 영역

    static Game* s_instance;
};

#endif