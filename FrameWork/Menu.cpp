#include "Include.h"

static DWORD KeyTime = GetTickCount();

Menu::Menu()
{
}

Menu::~Menu()
{
}

void Menu::Init()
{
	count = 0;
	alpha = 0;
	menuimg.Create("./resource/Img/lobby/Lobby.png", false, D3DCOLOR_XRGB(0, 0, 0));


}

// Chap, 재정의 함수 호출
void Menu::Update(double frame)
{

	//key.Update();

	//if (xx.xxx > 100 * xxx.xxx)
	//{
	//	if (GetTickCount() - CoinTime > 100)
	//	{
	//		count++;
	//		CoinTime = GetTickCount();
	//	}
	//	if (count > 1) count = 0;
	//}


}

void Menu::Draw()
{
	menuimg.Render(-50, 0, 0, 0.75, 0.75); //이미지출력
	dv_font.DrawString("로비", 200, 210);   //글자출력
	//if (GetTickCount() - AlTime > 100)
	//{
	//	alpha++;
	//	AlTime = GetTickCount();
	//}

	//if (alpha == 0) menuimg[2].SetColor(255, 255, 255, 255);
	//if (alpha == 1) menuimg[2].SetColor(255, 255, 255, 0);

	//if (xxx.xxx == 0) menuimg[2].Draw(407, 336);
	//else if (count < 2) menuimg[count].Draw(412, 276);

}

void Menu::OnMessage(MSG* msg)
{
	//char aaa[256];
	//_itoa_s(msg->message, aaa,2);
	//dv_font.DrawString(aaa, 10 , 210 ) ;   
	//PostQuitMessage(0);

	switch (msg->message)
	{
	case WM_KEYDOWN:
		switch (msg->wParam) {
		case VK_F12:
			//MessageBox(NULL, "", "", 0);
			if (g_Mng.n_Chap = MENU) {

				g_Mng.n_Chap = GAME; 
				/*sound.m_Bk1->Play(0, DSBPLAY_LOOPING);*/

			}
			break;
		}
		
	}

}