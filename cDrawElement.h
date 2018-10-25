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
	UINT GetObjectNum() { return (UINT)m_objects.size(); }

	template<typename T>
	vector<T*> GetObjectsPtr();

protected:
	shared_ptr<cRenderItem>		m_renderItem;
	vector<unique_ptr<cObject>>	m_objects;
	UINT						m_pickRender;
};

template<typename T>
inline vector<T*> cDrawElement::GetObjectsPtr()
{
	vector<T*> objectPtrs;

	for (auto& it : m_objects)
	{
		objectPtrs.push_back(static_cast<T*>(it.get()));
	}

	return objectPtrs;
}
