#pragma once

enum OBJECTSPLACE
{
	DRAW_PLNAES,
	DRAW_LINES,
	DRAW_DOTS,
	OBJECTS_COUNT,
};

class cDrawElement
{
public:
	cDrawElement();
	virtual ~cDrawElement();

	virtual void Update();
	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) = 0;
	virtual bool Picking(cObject** ppObject);
	virtual cObject* AddElement() = 0;

	void DeleteBackObject();

protected:
	shared_ptr<cRenderItem> m_renderItem;
	vector<unique_ptr<cObject>> m_objects;
};

