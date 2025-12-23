#pragma once
#include <d3d9.h>
#include <d3dx9.h>

class Sprite
{
public:
	Sprite(void);
	~Sprite(void);
 



	IDirect3DTexture9 *Texture;
	D3DXIMAGE_INFO imagesinfo;
	D3DCOLOR color;
	void SetTexture(LPDIRECT3DTEXTURE9 tex);

	LPD3DXSPRITE m_pSprite;  // Direct3D 스프라이트 객체 추가

	bool Create(const char* filename, bool bUseTransparency, D3DCOLOR TransparencyColor);

	void Draw( float dx , float dy , float sx , float sy , float sw , float sh, float centerX = 0, float centerY = 0 ); // #92

	void Draw(float x, float y); // #01

	//void Render( float x , float y , float radian, float sx, float sy);

	void Render(float x, float y, float radian, float sx, float sy, int pivotMode = 0);

	void RenderDraw( float x , float y , float sx , float sy , float sw , float sh, float radian, float sl, float st) ;

	void SetColor( int r , int g , int b , int a );
	void DrawUV(float x, float y, int frameIndex, int totalFrameCount);
};

extern Sprite g_Load;
