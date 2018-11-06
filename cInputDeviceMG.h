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

	void Update(const XMFLOAT3& cameraPos ,const XMFLOAT4X4& view,const XMFLOAT4X4& proj, float clientSizeX, float clientSizeY);
	void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool GetMouseOneDown(int vKey) const;
	bool GetMouseDownStay(int vKey) const;
	bool GetMouseUp(int vKey) const;
	void InputReset();

	PICKRAY XM_CALLCONV GetMousePickLay() const;
	POINT GetMousePosPt() const { return { (int)m_mousePos.x, (int)m_mousePos.y }; }
	XMFLOAT2 GetMousePosFlot() const { return { m_mousePos.x, m_mousePos.y }; }
	XMFLOAT3 GetMousePosFloat3() const { return { m_mousePos.x, m_mousePos.y, 0 }; }

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

