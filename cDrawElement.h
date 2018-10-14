#pragma once
class cDrawElement
{
public:
	cDrawElement();
	virtual ~cDrawElement();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) = 0;

protected:
	shared_ptr<cRenderItem> m_renderItem;
};

