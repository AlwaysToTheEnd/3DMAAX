#include "stdafx.h"

cButtonCollector::cButtonCollector()
	: m_buttonSize(0, 0)
	, m_pos(0, 0, 0)
	, m_renderItem(nullptr)
{
}


cButtonCollector::~cButtonCollector()
{
}

void cButtonCollector::SetUp(XMFLOAT2 buttonSize, shared_ptr<cRenderItem> renderItem)
{
	m_buttonSize = buttonSize;
	m_renderItem = renderItem;

	cUIObject::SetGeoAtRenderItem(renderItem);
}

void cButtonCollector::Update()
{
	XMMATRIX Tmat = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	for (auto& it : m_buttons)
	{
		it.Update(Tmat);
	}
}

void cButtonCollector::AddButton(UINT materTexIndex, function<void(int)> func)
{
	assert(m_renderItem && "It had not Setup");

	XMFLOAT2 pos = {0,0};
	pos.x += m_buttons.size()*m_buttonSize.x;

	m_buttons.push_back({});
	m_buttons.back().Build(m_renderItem);
	m_buttons.back().SetActiveFunc(func);
	m_buttons.back().SetMatrialIndex(materTexIndex);
	m_buttons.back().SetSize(m_buttonSize);
	m_buttons.back().SetPos({ pos.x,pos.y,m_pos.z });
}
