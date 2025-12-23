#include "Include.h"
#include "Global.h"

extern LPD3DXSPRITE g_sprite;

Camera cam;

//	유일 객체 (데이타 영역에 올라간 변수에 초기값 을 nullptr 로 셋팅)
//  이 로직은 프로그램 시작전에(main 호출) 실행된다.
Camera * Camera::m_pInstance = nullptr;

Camera* Camera::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		// 힙 메모리에 객체 할당 and 데이타 영역의 참조 변수에 주소 할당
		m_pInstance = new Camera();
	}

	return m_pInstance;
}


Camera::Camera()
{
	camPos.posX = 0;
	camPos.posY = 0;
}

Camera::~Camera()
{
}

void Camera::Update() // 원래는 모든 클래스를 다형성으로 배열로 참조하여 값을 전부 변경(리스트?)
{
	// camPos와 camScale을 기반으로 변환 행렬 생성
	D3DXMATRIX matScale, matTrans, matTransform;

	// 확대/축소 행렬
	D3DXMatrixScaling(&matScale, camScale, camScale, 1.0f);

	// 이동 행렬 (보통 카메라는 뷰 이동이므로 반대로 이동시킴)
	D3DXMatrixTranslation(&matTrans, -camPos.posX, -camPos.posY, 0);

	// 확대 후 이동
	matTransform = matScale * matTrans;



	// 만약 map 위치도 카메라 위치로 동기화한다면
	map.posX = camPos.posX;
	map.posY = camPos.posY;
}

void Camera::SetXCam(double x)
{
	camPos.posX = x;  // 누적이 아닌 절대값 대입
}

void Camera::SetYCam(double y)
{
	camPos.posY = y;
}
void Camera::SetScale(float scale)
{
	camScale = scale;  // 클래스 멤버 이름에 맞게 수정
	char buf[64];
	sprintf(buf, "Camera scale set to %.2f\n", scale);
	OutputDebugStringA(buf);
}

// Singleton 디자인 패턴으로 extern 선언 안해도 됨 (공부용)

void Camera::SetPosX(double x)
{
	camPos.posX = x;
	//char buf[64];
	//sprintf(buf, "Camera posX set to %.2f\n", (float)x);
	//OutputDebugStringA(buf);
}
void Camera::SetPosY(double y)
{
	camPos.posY = y;
	/*char buf[64];
	sprintf(buf, "Camera posY set to %.2f\n", (float)y);
	OutputDebugStringA(buf);*/
}
void Camera::ApplyTransform()
{
	D3DXMATRIX matScale, matTrans, matTransform;

	D3DXMatrixScaling(&matScale, camScale, camScale, 1.0f);
	D3DXMatrixTranslation(&matTrans, -camPos.posX, -camPos.posY, 0);
	matTransform = matScale * matTrans;

	g_sprite->SetTransform(&matTransform);
}
