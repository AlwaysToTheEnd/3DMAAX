#include "stdafx.h"

cInputDeviceMG* cInputDeviceMG::instance = nullptr;

cInputDeviceMG::cInputDeviceMG()
	: m_proj(MathHelper::Identity4x4())
	, m_view(MathHelper::Identity4x4())
	, m_clientSizeX(0)
	, m_clientSizeY(0)
	, m_mousePos(0, 0)
	, m_ray(0, 0, 0)
	, m_origin(0, 0, 0)
	, m_isLButtonDownStay(false)
	, m_isLButtonOneDown(0)
	, m_isLButtonUp(0)
	, m_isRButtonDownStay(false)
	, m_isRButtonOneDown(0)
	, m_isRButtonUp(0)
{
}


cInputDeviceMG::~cInputDeviceMG()
{
}

void cInputDeviceMG::Update(const XMFLOAT3& cameraPos, const XMFLOAT4X4 & view,const XMFLOAT4X4 & proj, float clientSizeX, float clientSizeY)
{
	m_view = view;
	m_proj = proj;
	m_clientSizeX = clientSizeX;
	m_clientSizeY = clientSizeY;

	m_origin = cameraPos;

	XMVECTOR ray = XMVectorSet((2.0f*m_mousePos.x / m_clientSizeX - 1.0f) / proj._11,
		(-2.0f*m_mousePos.y / m_clientSizeY + 1.0f) / proj._22, 1.0f, 0);

	XMMATRIX invViewMat = XMMatrixInverse(nullptr, XMLoadFloat4x4(&view));
	ray = XMVector3TransformNormal(ray, invViewMat);
	ray = XMVector3Normalize(ray);

	XMStoreFloat3(&m_ray, ray);

	if (m_isRButtonOneDown > 0)
	{
		m_isRButtonOneDown--;
	}

	if (m_isLButtonOneDown > 0)
	{
		m_isLButtonOneDown--;
	}

	if (m_isLButtonUp > 0)
	{
		m_isLButtonUp--;
	}

	if (m_isRButtonUp > 0)
	{
		m_isRButtonUp--;
	}
}

void cInputDeviceMG::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	{
		m_isLButtonDownStay = true;
		m_isLButtonOneDown = 2;
	}
	break;
	case WM_LBUTTONUP:
	{
		m_isLButtonDownStay = false;
		m_isLButtonUp = 2;
	}
	break;
	case WM_RBUTTONDOWN:
	{
		m_isRButtonDownStay = true;
		m_isRButtonOneDown = 2;
	}
	break;
	case WM_RBUTTONUP:
	{
		m_isRButtonDownStay = false;
		m_isRButtonUp = 2;
	}
	break;
	case WM_MOUSEMOVE:
	{
		m_mousePos.x = LOWORD(lParam);
		m_mousePos.y = HIWORD(lParam);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		//m_Distance -= GET_WHEEL_DELTA_WPARAM(wParam) / 100.0f;
	}
	break;
	}
}

bool cInputDeviceMG::GetMouseOneDown(int vKey) const
{
	switch (vKey)
	{
	case VK_LBUTTON:
		return m_isLButtonOneDown == 1;
		break;
	case VK_RBUTTON:
		return m_isRButtonOneDown == 1;
		break;
	default:
		assert(false);
		break;
	}

	return false;
}

bool cInputDeviceMG::GetMouseDownStay(int vKey) const
{
	switch (vKey)
	{
	case VK_LBUTTON:
		return m_isLButtonDownStay;
		break;
	case VK_RBUTTON:
		return m_isRButtonDownStay;
		break;
	default:
		assert(false);
		break;
	}

	return false;
}

bool cInputDeviceMG::GetMouseUp(int vKey) const
{
	switch (vKey)
	{
	case VK_LBUTTON:
		return m_isLButtonUp == 1;
		break;
	case VK_RBUTTON:
		return m_isRButtonUp == 1;
		break;
	default:
		assert(false);
		break;
	}

	return false;
}

void cInputDeviceMG::InputReset()
{
	m_isLButtonDownStay = 0;
	m_isLButtonUp = 0;
	m_isLButtonOneDown = 0;
	m_isRButtonDownStay = 0;
	m_isRButtonUp = 0;
	m_isRButtonOneDown = 0;
}

PICKRAY XM_CALLCONV cInputDeviceMG::GetMousePickLay() const
{
	return { XMLoadFloat3(&m_origin), XMLoadFloat3(&m_ray) };
}