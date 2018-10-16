#pragma once

class cButtonCollector
{
public:
	cButtonCollector();
	~cButtonCollector();

	void SetUp(XMFLOAT2 buttonSize, shared_ptr<cRenderItem> renderItem);
	void Update();
	void AddButton(UINT materTexIndex, function<void(int)> func);

	void SetPos(XMFLOAT3 pos) { m_pos = pos; }

private:
	XMFLOAT3 m_pos;
	XMFLOAT2 m_buttonSize;
	shared_ptr<cRenderItem> m_renderItem;
	vector<cUIButton> m_buttons;
};

