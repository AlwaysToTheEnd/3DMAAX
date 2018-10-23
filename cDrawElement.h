#pragma once

enum DRAWOBJECTSPLACE
{
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
	void SetPickRender(UINT key);
	UINT GetObjectNum() { return m_objects.size(); }

protected:
	shared_ptr<cRenderItem> m_renderItem;
	vector<unique_ptr<cObject>> m_objects;
};

