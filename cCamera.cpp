#include "stdafx.h"

cCamera::cCamera()
	: m_Distance(5)
	, m_eye(0, 0, 0)
	, m_isButtonDown(false)
	, m_PrevMouse{ 0,0 }
	, m_CurrMouse{ 0,0 }
	, m_RotX(0)
	, m_RotY(0)
	, m_View(MathHelper::Identity4x4())
{

}


cCamera::~cCamera()
{
}

void cCamera::Update(const XMFLOAT3 * pTarget)
{
	XMMATRIX matRotX = XMMatrixRotationX(m_RotX);
	XMMATRIX matRotY = XMMatrixRotationY(m_RotY);

	XMVECTOR eyePos = XMVectorSet(0, 0, -m_Distance, 1);

	eyePos = XMVector3TransformCoord(eyePos, matRotX*matRotY);
	XMVECTOR lookAt = XMVectorZero();


	if (pTarget)
	{
		lookAt = XMLoadFloat3(pTarget);
		eyePos = XMLoadFloat3(pTarget) + eyePos;
	}

	XMStoreFloat3(&m_eye, eyePos);
	XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(eyePos, lookAt, { 0,1,0,0 }));
}

void cCamera::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_RBUTTONDOWN:
	{
		m_PrevMouse.x = LOWORD(lParam);
		m_PrevMouse.y = HIWORD(lParam);
		m_isButtonDown = true;
	}
	break;
	case WM_RBUTTONUP:
	{
		m_isButtonDown = false;
	}
	break;
	case WM_MOUSEMOVE:
	{
		m_CurrMouse.x = LOWORD(lParam);
		m_CurrMouse.y = HIWORD(lParam);
		if (m_isButtonDown)
		{
			POINT ptCurrMouse;
			ptCurrMouse.x = LOWORD(lParam);
			ptCurrMouse.y = HIWORD(lParam);

			m_RotX += (ptCurrMouse.y - m_PrevMouse.y) / 100.0f;
			m_RotY += (ptCurrMouse.x - m_PrevMouse.x) / 100.0f;

			if (m_RotX <= -MathHelper::Pi * 0.5f + FLT_EPSILON)
				m_RotX = -MathHelper::Pi * 0.5f + FLT_EPSILON;
			else if (m_RotX >= MathHelper::Pi * 0.5f - FLT_EPSILON)
				m_RotX = MathHelper::Pi * 0.5f - FLT_EPSILON;

			m_PrevMouse = ptCurrMouse;
		}
	}
	break;
	case WM_MOUSEWHEEL:
	{
		m_Distance -= GET_WHEEL_DELTA_WPARAM(wParam) / 100.0f;
	}
	break;
	}
}