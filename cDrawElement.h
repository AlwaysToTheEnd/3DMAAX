#pragma once

enum DRAWELEMNTPLACE
{
	DRAW_PLNAES,
	DRAW_LINES,
	DRAW_DOTS,
	DRAW_COUNT,
};

class cDrawElement
{
public:
	cDrawElement();
	virtual ~cDrawElement();

	virtual void Update() = 0;
	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) = 0;

protected:
	shared_ptr<cRenderItem> m_renderItem;
};

