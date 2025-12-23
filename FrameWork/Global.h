#pragma once
#define WIN32_LEAN_AND_MEAN   // windows.h가 winsock.h 포함 못하게 막음
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "BossSkeleton.h"


extern RECT g_mapBoundary;             // 전역 변수 선언
extern LPDIRECT3DTEXTURE9 g_goldTex;
extern LPDIRECT3DTEXTURE9 g_hpPotionTex;
extern LPDIRECT3DDEVICE9 g_pd3dDevice;
extern bool g_showColliderBox;
extern BossSkeleton* boss;
