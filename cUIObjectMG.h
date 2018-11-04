#pragma once

#define UIMG cUIObjectMG::Get()
	
class cUIObjectMG
{
public:
	~cUIObjectMG();

	static cUIObjectMG * Get()
	{
		if (instance == nullptr)
		{
			instance = new cUIObjectMG;
		}

		return instance;
	}

	void Build();
	void Update();
	void UIOn(string key);
	void UIOn(cUIObject* object);
	void UIOff(string key);
	void UIOff(cUIObject* object);

	template<typename T>
	T* GetUI(string key);

	template<typename T>
	T* AddUI(string key);

private:
	cUIObjectMG();

	void OnListCheck(cUIObject* object, bool on_off);

private:
	static cUIObjectMG*		instance;

	MeshGeometry*			m_UIGeo;
	shared_ptr<cRenderItem>	m_UIRender;
	list<cUIObject*>		m_OnUIs;
	cUIObject*				m_currPickUI;
	unordered_map<string, unique_ptr<cUIObject>> m_UIObjects;
};

template<typename T>
inline T * cUIObjectMG::GetUI(string key)
{
	auto it = m_UIObjects.find(key);

	if (it == m_UIObjects.end())
	{
		return nullptr;
	}
	else
	{
		return static_cast<T*>(it->second.get());
	}
}

template<typename T>
inline T * cUIObjectMG::AddUI(string key)
{
	auto newUIObject = make_unique<T>();
	newUIObject->Build(m_UIRender);

	newUIObject->SetUIName(key);

	m_UIObjects.insert({ key, move(newUIObject) });

	return static_cast<T*>(m_UIObjects[key].get());
}
