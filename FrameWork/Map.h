#pragma once
#include "Include.h"

enum class MapType {
    Entrance = 0,
    Battle = 1,
    Boss = 2
};

class Map
{

public :
	Map();
	~Map();

    void DrawAtOffset(int dx, int dy);  // 슬라이드 시 오프셋 적용해서 그리기
  
    Sprite m_Back;          // 단일 배경 이미지
   
    const std::vector<RECT>& GetObstacleRects() const { return m_obstacleRects; }
    int    m_Stage = 1;
    double posX = 0, posY = 0;

    void Init(MapType type);  //  MapType으로 초기화
    void Update(double frame);
    void Draw();

    MapType GetType() const { return m_mapType; }


private:
    MapType m_mapType = MapType::Battle;  // 기본값: 전투맵
    std::vector<RECT> m_obstacleRects;
};

extern Map map;