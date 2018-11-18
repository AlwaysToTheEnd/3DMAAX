#include "stdafx.h"

cUIObjectMG* cUIObjectMG::instance = nullptr;

cUIObjectMG::cUIObjectMG()
	: m_UIGeo(nullptr)
	, m_UIRender(nullptr)
	, m_currPickUI(nullptr)
{

}

void cUIObjectMG::OnListCheck(cUIObject * object, bool on_off)
{
	if (on_off)
	{
		bool isHad = false;
		for (auto it = m_OnUIs.begin(); it != m_OnUIs.end(); it++)
		{
			if (object == *it)
			{
				isHad = true;
				break;
			}
		}

		if (!isHad)
		{
			m_OnUIs.push_front(object);
		}
	}
	else
	{
		for (auto it = m_OnUIs.begin(); it != m_OnUIs.end(); it++)
		{
			if (object == *it)
			{
				m_OnUIs.erase(it);
				break;
			}
		}
	}
}


cUIObjectMG::~cUIObjectMG()
{

}


void cUIObjectMG::Build()
{
	vector<NT_Vertex> vertices(4);

	vertices[0] = { {0,0,0},{0,0,-1},{0,0} };
	vertices[1] = { {1,0,0},{0,0,-1},{1,0} };
	vertices[2] = { {1,1,0},{0,0,-1},{1,1} };
	vertices[3] = { {0,1,0},{0,0,-1},{0,1} };

	vector<UINT16> Indices;
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);
	Indices.push_back(0);
	Indices.push_back(2);
	Indices.push_back(3);

	const UINT uiGeoDataSize = sizeof(NT_Vertex)*(UINT)vertices.size();
	const UINT uiGeoIndexSize = sizeof(UINT16)*(UINT)Indices.size();

	m_UIGeo = MESHMG->AddMeshGeometry("ui", vertices.data(), Indices.data(),
		sizeof(NT_Vertex), uiGeoDataSize, DXGI_FORMAT_R16_UINT, uiGeoIndexSize, false);

	m_UIRender = RENDERITEMMG->AddRenderItem("ui");
	m_UIRender->SetGeometry(m_UIGeo, "ui");
}

void cUIObjectMG::Update()
{
	XMMATRIX mat = XMMatrixIdentity();
	bool isUIClickCheck = false;

	if (INPUTMG->GetMouseOneDown(VK_LBUTTON) || INPUTMG->GetMouseOneDown(VK_RBUTTON))
	{
		isUIClickCheck = true;
	}

	if (isUIClickCheck)
	{
		bool mouseInUI = false;
		for (auto& it : m_OnUIs)
		{
			if (it->IsMousePosInUI())
			{
				if (!mouseInUI)
				{
					mouseInUI = true;
					m_currPickUI = it;
				}
			}

			it->Update(mat);
		}

		if (!mouseInUI)
		{
			m_currPickUI = nullptr;
		}
		else
		{
			OnListCheck(m_currPickUI, false);
			m_OnUIs.push_front(m_currPickUI);
			m_UIRender->ThisInstanceIsEndRender(m_currPickUI->m_renderInstance);
		}
	}
	else
	{
		for (auto& it : m_OnUIs)
		{
			it->Update(mat);
		}
	}

	if (m_currPickUI)
	{
		m_currPickUI->InputDataUpdate();
		INPUTMG->InputReset();

		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
		{
			m_currPickUI = nullptr;
		}
	}
}

void cUIObjectMG::UIOn(string key)
{
	auto iter = m_UIObjects.find(key);

	if (iter == m_UIObjects.end()) assert(false);

	m_UIRender->ThisInstanceIsEndRender(iter->second->m_renderInstance);
	OnListCheck(iter->second.get(), true);
	iter->second->SetRenderState(true);
}

void cUIObjectMG::UIOn(cUIObject * object)
{
	m_UIRender->ThisInstanceIsEndRender(object->m_renderInstance);
	OnListCheck(object, true);
	object->SetRenderState(true);
}

void cUIObjectMG::UIOff(string key)
{
	auto iter = m_UIObjects.find(key);

	if (iter == m_UIObjects.end()) assert(false);

	OnListCheck(iter->second.get(), false);

	if (m_currPickUI == iter->second.get())
	{
		m_currPickUI = nullptr;
	}

	iter->second->SetRenderState(false);
}

void cUIObjectMG::UIOff(cUIObject * object)
{
	if (m_currPickUI == object)
	{
		m_currPickUI = nullptr;
	}

	OnListCheck(object, false);
	object->SetRenderState(false);
}
