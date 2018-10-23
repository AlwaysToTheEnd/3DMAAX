#pragma once

class cButtonCollector
{
public:
	cButtonCollector();
	~cButtonCollector();

	void SetUp(XMFLOAT2 buttonSize, shared_ptr<cRenderItem> renderItem);
	void Update();
	void AddButton(UINT materTexIndex, function<void(int)> func, UINT parameterValue);

	void SetPos(XMFLOAT3 pos) { m_pos = pos; }
	void SetRenderState(bool value);

	UINT GetButtonNum() { return m_buttons.size(); }

private:
	XMFLOAT3 m_pos;
	XMFLOAT2 m_buttonSize;
	shared_ptr<cRenderItem> m_renderItem;
	vector<unique_ptr<cUIButton>> m_buttons;
};

