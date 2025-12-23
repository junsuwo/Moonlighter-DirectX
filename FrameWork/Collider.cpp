#include "Include.h"


extern LPDIRECT3DDEVICE9 g_pd3dDevice;  // Direct3D 디바이스
extern LPD3DXSPRITE g_sprite;           // 전역 스프라이트


Collider coll;


Collider::Collider()
{

}

Collider::~Collider()
{
    
    if (m_fillTex) {
        m_fillTex->Release();
        m_fillTex = nullptr;
    }

}

void Collider::Init()
{
	m_rc.left = 0;
	m_rc.top = 0;
	m_rc.right = 1250;
	m_rc.bottom = 720;

    D3DXCreateTexture(g_pd3dDevice, 1, 1, 1, 0,
        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_fillTex);

    if (m_fillTex) {
        D3DLOCKED_RECT lockedRect;
        if (SUCCEEDED(m_fillTex->LockRect(0, &lockedRect, nullptr, 0))) {
            DWORD* pixels = (DWORD*)lockedRect.pBits;
            pixels[0] = D3DCOLOR_ARGB(255, 255, 255, 255);
            m_fillTex->UnlockRect(0);
        }
    }


}

void Collider::Update()
{
}

// 바닥
void Collider::Draw()
{
	if (Gmanager.m_GameStart == true)
	{
		dv_font.DrawString("┌ ", m_rc.left, m_rc.top, D3DCOLOR_ARGB(255, 0, 255, 0));
		dv_font.DrawString(" ┐", m_rc.right, m_rc.top, D3DCOLOR_ARGB(255, 0, 255, 0));
		dv_font.DrawString("└ ", m_rc.left, m_rc.bottom, D3DCOLOR_ARGB(255, 0, 255, 0));
		dv_font.DrawString(" ┘ ", m_rc.right, m_rc.bottom, D3DCOLOR_ARGB(255, 0, 255, 0));
	}

}

// 디버그 콜라이더 (x, y는 오프셋)
void Collider::BoxSow(RECT m_rc, long x, long y, D3DCOLOR color)
{
	if (Gmanager.m_GameStart == true /* && 디버그 일때 처리 */)
	{
		dv_font.DrawString("┌ ", m_rc.left-x, m_rc.top  - y, color);
		dv_font.DrawString(" ┐ ", m_rc.right+x, m_rc.top - y, color);
		dv_font.DrawString("└ ", m_rc.left-x, m_rc.bottom + y, color);
		dv_font.DrawString(" ┘ ", m_rc.right+x, m_rc.bottom + y, color);
	}

}


void Collider::FillRect(int x, int y, int width, int height, D3DCOLOR color) {
    if (!m_fillTex) return;

    D3DXVECTOR2 scale((float)width, (float)height);
    D3DXVECTOR2 pos((float)x, (float)y);
    D3DXVECTOR2 center(0, 0);

    D3DXMATRIX mat;
    D3DXMatrixTransformation2D(&mat, nullptr, 0.0f, &scale, &center, 0.0f, &pos);

    g_sprite->Begin(D3DXSPRITE_ALPHABLEND);
    g_sprite->SetTransform(&mat);
    g_sprite->Draw(m_fillTex, nullptr, nullptr, nullptr, color);
    g_sprite->End();

    D3DXMATRIX identity;
    D3DXMatrixIdentity(&identity);
    g_sprite->SetTransform(&identity);
}