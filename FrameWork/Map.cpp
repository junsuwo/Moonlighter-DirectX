#include "Include.h"

Map map;


Map::Map()
{
	
}

Map::~Map()
{
}

void Map::Init(MapType type)
{
	
	m_mapType = type;

	m_Back.Create("./resource/Img/map1/Dungeon/dungeon_background.png", false, D3DCOLOR_XRGB(0, 0, 0));

	// 맵 타입별 장애물 초기화 예시 (직접 영역 하드코딩)
	m_obstacleRects.clear();

	if (type == MapType::Battle) {
		// 예: 벽 영역 4개 박스 추가
		m_obstacleRects.push_back({ 0, 0, 50, 768 });         // 왼쪽 벽
		m_obstacleRects.push_back({ 1230, 0, 1280, 768 });    // 오른쪽 벽
		m_obstacleRects.push_back({ 0, 0, 1280, 50 });        // 위쪽 벽
		m_obstacleRects.push_back({ 0, 718, 1280, 768 });     // 아래쪽 벽
		// 추가 장애물 영역 필요하면 push_back 추가
	}
	else if (type == MapType::Entrance) {
		// Entrance 맵 장애물 영역
	}
	else if (type == MapType::Boss) {
		// Boss 맵 장애물 영역
	}
}

void Map::Update(double frame)
{
	
}

void Map::Draw()
{
	

	float scaleX = 1280.0f / 1600.0f;
	float scaleY = 768.0f / 900.0f;

	float scaledWidth = 1600.0f * scaleX; // = 1280
	float scaledHeight = 900.0f * scaleY; // ≈ 768

	float offsetX = (1280.0f - scaledWidth) / 2.0f; // = 0
	float offsetY = (768.0f - scaledHeight) / 2.0f; // = 0

	m_Back.Render(offsetX, offsetY, 0.0f, scaleX, scaleY, 0);
}
void Map::DrawAtOffset(int dx, int dy)
{
	float scaleX = 1280.0f / 1600.0f;
	float scaleY = 768.0f / 900.0f;

	float scaledWidth = 1600.0f * scaleX; // = 1280
	float scaledHeight = 900.0f * scaleY; // ≈ 768

	float offsetX = (1280.0f - scaledWidth) / 2.0f; // = 0
	float offsetY = (768.0f - scaledHeight) / 2.0f; // = 0

	// posX, posY가 0이라면 offsetX, offsetY만 위치 조정용으로 쓰는 것 같으니 같이 더해줌
	float drawX = offsetX + static_cast<float>(dx) + static_cast<float>(posX);
	float drawY = offsetY + static_cast<float>(dy) + static_cast<float>(posY);

	// m_Back.Render는 (x, y, rotation, scaleX, scaleY, 색상) 이런 형태라고 가정
	m_Back.Render(drawX, drawY, 0.0f, scaleX, scaleY, 0);
}