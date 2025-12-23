#pragma once
#include"Include.h"


// UI 매니저 클래스: 게임 내 UI를 총괄 관리 (체력바, 골드, 포션, 인벤토리 등)
class UIManager
{
public:
    UIManager();
    ~UIManager();

    // Direct3D 디바이스를 세팅 (초기화 시 호출)
    void Init(LPDIRECT3DDEVICE9 device);




    // 리소스 해제
    void Release();

    // 체력 관련 함수
    void SetHP(int currentHP, int maxHP);  // 체력 최대/현재값 설정
    void DrawBossHpBar(float currentHp, float maxHp, int x, int y);
    // 골드 관련 함수
    void SetGold(int gold);         // 골드 현재값 설정
    void AddGold(int amount);       // 골드 더하기
    int GetGold() const;            // 골드 현재값 반환

    // 포션 관련 함수
    void AddPotion(int amount);     // 포션 개수 더하기
    int GetPotionCount() const;     // 포션 개수 반환

    // 포션 사용 함수 선언
    void UsePotionIfAvailable(Hero& hero);


    // 매 프레임 UI를 그리는 함수
    void Render();

    // 인벤토리 관련 함수
    void DrawInventory();           // 인벤토리 UI 그리기
    void ToggleInventory();         // 인벤토리 열기/닫기 토글
    bool IsInventoryOpen() const;  // 인벤토리 열림 여부 반환

    D3DXVECTOR2 GetInventorySlotPosition(DropType type);

    void AddItemToInventory(DropType type, int amount = 1);  // 인벤토리에 아이템 추가

    void SetPotionTexture(LPDIRECT3DTEXTURE9 tex) { m_potionTex = tex; }  // 포션 텍스처 세팅

    void DrawDarkOverlay();         // 인벤토리 열렸을 때 화면 어둡게 덮기 (다크 오버레이)

    // 인벤토리 커서 관련 함수
    void UpdateInventoryCursor();  // 커서 위치 업데이트 (키보드 입력 처리)
    void DrawInventoryCursor();    // 커서 테두리 그리기
    void UseSelectedItem(Hero& hero);        // 선택된 아이템 사용 (포션 등)

private:
    // 인벤토리 슬롯 위치 초기화
    void InitInventorySlots();
    bool m_prevW = false;
    bool m_prevA = false;
    bool m_prevS = false;
    bool m_prevD = false;

    // 인벤토리 커서 좌표 및 열 수 (4행 x 5열 인벤토리 기준)
    int m_cursorX = 0;         // 현재 커서 X 위치 (0~4)
    int m_cursorY = 0;         // 현재 커서 Y 위치 (0~3)
    const int m_inventoryCols = 5;  // 한 행의 슬롯 수 (열 개수)



    // 디버그용 인벤토리 슬롯 위치 박스 그리기
    void DebugDrawInventorySlots();

    static constexpr int INVENTORY_SLOT_COUNT = 20;  // 인벤토리 슬롯 총 개수 (4x5)

    // 인벤토리 한 칸에 해당하는 데이터 구조체 (아이템 종류, 수량)
    struct InventorySlot {
        DropType type = DropType::NONE;
        int quantity = 0;
    };

    // Direct3D 디바이스 (그래픽 출력용)
    LPDIRECT3DDEVICE9 m_device = nullptr;

    // 체력바 관련 텍스처
    LPDIRECT3DTEXTURE9 m_hpBarBackTex = nullptr;   // 체력바 배경 텍스처
    LPDIRECT3DTEXTURE9 m_hpBarFrontTex = nullptr;  // 체력바 전면 (체력양) 텍스처
    LPDIRECT3DTEXTURE9 m_hartBackTex = nullptr;    // 체력 하트 아이콘 배경 텍스처

    LPDIRECT3DTEXTURE9 m_bossHpBarBackTex = nullptr;   // 흰색 배경 텍스처
    LPDIRECT3DTEXTURE9 m_bossHpBarFrontTex = nullptr;  // 빨간색 전면 텍스처



    // 오른쪽 UI 인터페이스 텍스처
    LPDIRECT3DTEXTURE9 m_rightTopTex = nullptr;
    LPDIRECT3DTEXTURE9 m_rightBotTex = nullptr;

    // 골드 관련 텍스처
    LPDIRECT3DTEXTURE9 m_goldBackTex = nullptr;    // 골드 배경 이미지
    LPDIRECT3DTEXTURE9 m_goldFrames[6];            // 골드 단계별 텍스처 6장 (프레임별)

    // 포션 텍스처
    LPDIRECT3DTEXTURE9 m_potionTex = nullptr;

    // 인벤토리 UI 텍스처 및 스프라이트
    LPDIRECT3DTEXTURE9 m_inventoryTex = nullptr;
    Sprite m_inventorySprite;          // 인벤토리 UI 스프라이트

    LPDIRECT3DTEXTURE9 m_blackTex = nullptr;       // 1x1 검은색 텍스처 (다크 오버레이용)
    Sprite m_blackSprite;                           // 검은색 스프라이트 (투명도 조절 가능)

    // 인벤토리 열림 여부 플래그
    bool m_inventoryOpen = false;

    // 인벤토리 슬롯 데이터 배열 (20칸)
    InventorySlot m_slots[INVENTORY_SLOT_COUNT];

    // 슬롯별 위치(화면 좌표)를 저장하는 벡터
    std::vector<RECT> m_inventorySlots;

    // 현재 선택된 슬롯 인덱스 (커서 위치)
    int m_selectedSlotIndex = 0;

    // 커서 스프라이트 및 텍스처 (인벤토리 선택 표시용)
    Sprite m_cursorSprite;
    LPDIRECT3DTEXTURE9 m_cursorTex = nullptr;

    // UI 텍스트 렌더용 작은 폰트 객체
    LPD3DXFONT m_smallFont = nullptr;
    LPDIRECT3DTEXTURE9 m_itemNameTex = nullptr;      // 배경 이미지 (700x54)
    LPDIRECT3DTEXTURE9 m_nameHpPotionTex = nullptr;  // "HP 포션" 텍스트 이미지 (160x24)


    // 체력바 렌더용 스프라이트 객체
    Sprite m_hpBarBackSprite;
    Sprite m_hpBarFrontSprite;
    Sprite m_hartBackSprite;

    Sprite m_bossHpBarBackSprite;
    Sprite m_bossHpBarFrontSprite;


    // 골드 UI 렌더용 스프라이트
    Sprite m_goldBackSprite;
    Sprite m_goldFrameSprites[6];      // 각 골드 프레임별 스프라이트

    // 오른쪽 UI 렌더용 스프라이트
    Sprite m_rightTopSprite;
    Sprite m_rightBotSprite;

    // 체력 현재값 / 최대값 저장
    int m_currentHP = 100;
    int m_maxHP = 100;

    // 골드와 포션 보유량 저장 변수
    int m_gold = 0;
    int m_potionCount = 0;

 
};