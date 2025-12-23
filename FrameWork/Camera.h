#pragma once

struct CameraPos
{
	double posX;
	double posY;

};

class Camera
{
private:
	static Camera* m_pInstance;
	CameraPos camPos;
	float camScale = 1.0f;

public:
	Camera();
	~Camera();

	static Camera* GetInstance();

	void SetXCam(double);
	void SetYCam(double);

	void SetScale(float scale);
	float GetScale() const { return camScale; }

	double GetPosX() const { return camPos.posX; }
	double GetPosY() const { return camPos.posY; }
	void SetPosX(double x);
	void SetPosY(double y);

	void GetTransformMatrix(D3DXMATRIX* outMat)
	{
		D3DXMATRIX matScale, matTrans;
		D3DXMatrixScaling(&matScale, camScale, camScale, 1.0f);
		D3DXMatrixTranslation(&matTrans, -(float)camPos.posX, -(float)camPos.posY, 0.0f);
		*outMat = matScale * matTrans;
	}

	void ApplyTransform();
	void Update();
};