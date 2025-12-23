//#include "Include.h"
//
//
//Player player;
//
//
//Player::Player()
//{
//	pos.x = 700;
//	pos.y = 300;
//	gravity = 5.75f;
//	grounded = false;
//
//}
//
//Player::~Player()
//{
//}
//
//void Player::Init()
//{
//
//	char FileName[256];
//	
//
//	sprintf_s(FileName, "./resource/Img/7.png");
//	playerimg.Create(FileName,false,D3DCOLOR_XRGB(0,0,0));
//	D3DXGetImageInfoFromFile(FileName, &imagesinfo);
//
//	m_rc.left = pos.x;
//	m_rc.top = pos.y;
//	m_rc.right = pos.x+imagesinfo.Width+10;
//	m_rc.bottom = pos.y+imagesinfo.Height+30;
//
//}
//
//void Player::Update()
//{
//	if(GetTickCount() - m_PlayerTime > 10)
//	{
//		if(!grounded) pos.y += gravity;
//
//		if (m_rc.left < coll.m_rc.right && coll.m_rc.left < m_rc.right && m_rc.top < coll.m_rc.bottom && coll.m_rc.top < m_rc.bottom) grounded = true;
//
//		m_rc.left = pos.x;
//		m_rc.top = pos.y;
//		m_rc.right = pos.x + imagesinfo.Width + 10;
//		m_rc.bottom = pos.y + imagesinfo.Height + 30;
//
//		m_PlayerTime = GetTickCount();
//	}
//}
//
//void Player::Draw()
//{
//	if (Gmanager.m_GameStart == true)
//	{
//		playerimg.Render(pos.x, pos.y, 0, 2, 2);
//		if (grounded)
//		{
//			coll.BoxSow(m_rc, 0, -5, 0xffff0000);
//		}
//		else
//		{
//			coll.BoxSow(m_rc, 0, -5);
//		}
//	}
//
//}

/*

	기본 충돌처리

	= 사각형 출동 판정 처리
	플레이어기의 충돌 판정 좌표를 (ML, MT) ~ (MR, MB), 탄의 충돌 판정 좌표를 (BL, BT) ~ (BR, BB)라고 해보자.
	(ML, MT), (BL, BT)는 각각의 사각형의 왼쪽 위 좌표이고 (MR. MB), (BR, BB)는 사각형의 오른쪽 아래 좌표이다.
	이 경우, 플레이어기에 탄의 충돌 조건은 다음과 같다.
	* ML < BR && BL < MR && MT < BB && BT < MB

	= 원을 이용한 충돌 판정 처리
	원을 이용한 충돌 판정 처리는 다음과 같다.
	플레이어기의 중심좌표를 (MX, MY), 탄의 중심좌표를 (BX, BY)라고 하고, 플레이어기의 충돌 판정 반경을 MR,
	탄의 충돌 판정 반경을 BR이라고 하자. 이때 플레이어기와 탄의 충돌 조건은 아래와 같다.
	* (MX-BX)*(MX-BX) + (MY-BY)*(MY-BY) < (MR+BR)*(MR+BR)

*/