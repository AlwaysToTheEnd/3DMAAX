#pragma once
#define INPUTMG cInputDeviceMG::Get()

class cInputDeviceMG
{
public:
	static cInputDeviceMG * Get()
	{
		if (instance == nullptr)
		{
			instance = new cInputDeviceMG;
		}

		return instance;
	}

	~cInputDeviceMG();

	void Update(XMFLOAT4X4& view, XMFLOAT4X4& proj, float clientSizeX, float clientSizeY);
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool GetMouseOneDown(int vKey);
	bool GetMouseDownStay(int vKey);
	bool GetMouseUp(int vKey);

	PICKRAY XM_CALLCONV GetMousePickLay();
	POINT GetMousePosPt() { return { (int)m_mousePos.x, (int)m_mousePos.y }; }
	XMFLOAT2 GetMousePosFlot() { return { m_mousePos.x, m_mousePos.y }; }

private:
	cInputDeviceMG();

private:
	static cInputDeviceMG* instance;
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
	float m_clientSizeX;
	float m_clientSizeY;
	XMFLOAT2 m_mousePos;
	XMFLOAT3 m_ray;
	XMFLOAT3 m_origin;

	bool m_isLButtonDownStay;
	int m_isLButtonUp;
	int m_isLButtonOneDown;
	bool m_isRButtonDownStay;
	int m_isRButtonUp;
	int m_isRButtonOneDown;
};

