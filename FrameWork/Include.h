#pragma once

/* ---------------------------------------------------
///////////////// 프로그램 파일 //////////////////////

		라이브러리에 필요한 파일입니다.
		지우면 안돼요~~^^;

//////////////////////////////////////////////////////
-----------------------------------------------------*/

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dsound.lib")

// 데이타 베이스///////////////////
//#include <my_global.h>
#include <winsock2.h>
//#include <mysql.h>
//#pragma comment(lib, "libmySQL.lib")

//////////////////////////////////

#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include "DXUTsound.h"

#include <list>

#include "Define.h"
#include "DeviceFont.h"
#include "Sprite.h"
#include "Chap.h"
#include "Hero.h"
#include "Door.h"
#include "Monster.h"

/* ---------------------------------------------------
///////////////// 사용자 추가 파일 ///////////////////

		새로 클래스 만들 때 마다
		헤더파일을 추가해 주세요~

//////////////////////////////////////////////////////
-----------------------------------------------------*/
/* ========= 화면경계 ============== */
const int SCREEN_LEFT = 0;
const int SCREEN_TOP = 0;
const int SCREEN_RIGHT = 1280;
const int SCREEN_BOTTOM = 768;
#define FRAME_W 120     // 각 프레임의 너비 (px)
#define FRAME_H 120     // 각 프레임의 높이 (px)
/* ================================= */

// 두 RECT 사각형이 충돌(겹침)했는지 확인하는 함수
// ★ 인라인 함수로 정의 (중복 정의 방지)
inline bool CheckCollision(const RECT& a, const RECT& b)
{
	return (a.left < b.right && a.right > b.left &&
		a.top < b.bottom && a.bottom > b.top);
}



#include "Game.h"
#include "Logo.h"
#include "Menu.h"
#include "Over.h"
#include "CMng.h"

//#include "Night.h"

#include "Camera.h"

#include "Map.h"

#include "Key.h"
//#include "Sound.h"


#include "Collider.h"

#include "GameManager.h"
// 데이타 베이스///////////////////
#include "Mysql.h"

/* ---------------------------------------------------
///////////////// 사용자 추가 파일 ///////////////////

		키 입력 처리 함수입니다.

//////////////////////////////////////////////////////
-----------------------------------------------------*/

inline int KeyDown	( int vk ) 
{ 
	return ( (GetAsyncKeyState(vk) & 0x8000) ? 1:0 ); 
}

inline int KeyUp	( int vk ) 
{ 
	return ( (GetAsyncKeyState(vk) & 0x8000) ? 0:1 ); 
}