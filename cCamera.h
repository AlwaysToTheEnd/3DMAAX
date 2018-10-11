#pragma once


class cCamera
{
public:
	cCamera();
	~cCamera();

	void Update(const XMFLOAT3* pTarget=nullptr);
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	const XMFLOAT4X4* GetViewMatrix() { return &m_View; }
	const XMFLOAT3&	GetEyePos() { return m_eye; }
	PICKRAY XM_CALLCONV GetMousePickLay(XMFLOAT4X4& projMat,float clientSizeX,float clientSizeY);

private:
	XMFLOAT3	m_eye;

	XMFLOAT4X4	m_View;

	float		m_Distance;
	float		m_RotX;
	float		m_RotY;
	bool		m_isButtonDown;
	POINT		m_PrevMouse;
	POINT		m_CurrMouse;
};

