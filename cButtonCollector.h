#pragma once

class cButtonCollector
{
public:
	cButtonCollector();
	~cButtonCollector();

	void Build(XMFLOAT2 buttonSize);
	void Update();
	void AddButton(string buttonName, UINT materTexIndex, function<void(int)> func, UINT parameterValue);

	void SetPos(XMFLOAT3 pos) { m_pos = pos; }
	void SetRenderState(bool value);

	UINT GetButtonNum() { return (UINT)m_buttons.size(); }

private:
	XMFLOAT3 m_pos;
	XMFLOAT2 m_buttonSize;
	vector<cUIButton*> m_buttons;
};

