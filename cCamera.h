#pragma once


class cCamera
{
public:
	cCamera();
	~cCamera();

	void Update();
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	const XMFLOAT4X4* GetViewMatrix() { return &m_View; }
	const XMFLOAT3&	GetEyePos() { return m_eye; }
	void SetTarget(const cObject* target) { m_target = target; }
	void SetTargetAndSettingAngle(const cObject* target);

private:
	XMFLOAT3		m_eye;
	XMFLOAT4X4		m_View;
	const cObject*	m_target;

	float		m_Distance;
	float		m_RotX;
	float		m_RotY;
	float		m_RotZ;
	bool		m_isButtonDown;
	POINT		m_PrevMouse;
	POINT		m_CurrMouse;
};

