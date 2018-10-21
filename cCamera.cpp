#include "stdafx.h"

cCamera::cCamera()
	: m_Distance(5)
	, m_target(nullptr)
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

void cCamera::Update()
{
	XMMATRIX matRot = XMMatrixRotationRollPitchYaw(m_RotX, m_RotY, m_RotZ);
	XMVECTOR eyePos = XMVectorSet(0, 0, -m_Distance, 1);

	eyePos = XMVector3TransformNormal(eyePos, matRot);
	XMVECTOR lookAt = XMVectorZero();

	if (m_target)
	{
		lookAt = XMLoadFloat3(&m_target->GetWorldPos());
		eyePos = XMLoadFloat3(&m_target->GetWorldPos()) + eyePos;
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

void cCamera::SetTargetAndSettingAngle(const cObject * target)
{
	m_target = target;
	XMFLOAT4 rotationQuater = m_target->GetQuaternionInstance();

	double q2sqr = rotationQuater.y * rotationQuater.y;
	double t0 = -2.0 * (q2sqr + rotationQuater.z * rotationQuater.z) + 1.0;
	double t1 = +2.0 * (rotationQuater.x * rotationQuater.y + rotationQuater.w * rotationQuater.z);
	double t2 = -2.0 * (rotationQuater.x * rotationQuater.z - rotationQuater.w * rotationQuater.y);
	double t3 = +2.0 * (rotationQuater.z * rotationQuater.z + rotationQuater.w * rotationQuater.x);
	double t4 = -2.0 * (rotationQuater.x * rotationQuater.x + q2sqr) + 1.0;

	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;

	m_RotX = asin(t2);
	m_RotY = atan2(t3, t4);
	m_RotZ = atan2(t1, t0);
}
