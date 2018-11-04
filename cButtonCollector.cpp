#include "stdafx.h"

cButtonCollector::cButtonCollector()
	: m_buttonSize(0, 0)
	, m_pos(0, 0, 0)
{
}


cButtonCollector::~cButtonCollector()
{
}

void cButtonCollector::Build(XMFLOAT2 buttonSize)
{
	m_buttonSize = buttonSize;
}

void cButtonCollector::Update()
{
	XMMATRIX Tmat = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	for (auto& it : m_buttons)
	{
		it->Update(Tmat);
	}
}

void cButtonCollector::AddButton(string buttonName, UINT materTexIndex, function<void(int)> func, UINT parameterValue)
{
	XMFLOAT2 pos = {0,0};
	pos.x += m_buttons.size()*m_buttonSize.x;

	m_buttons.push_back(UIMG->AddUI<cUIButton>(buttonName));
	m_buttons.back()->SetActiveFunc(func);
	m_buttons.back()->SetParameter(parameterValue);
	m_buttons.back()->SetMatrialIndex(materTexIndex);
	m_buttons.back()->SetSize(m_buttonSize);
	m_buttons.back()->SetPos({ pos.x,pos.y,m_pos.z });
}

void cButtonCollector::SetRenderState(bool value)
{
	if (value)
	{
		for (auto& it : m_buttons)
		{
			UIMG->UIOn(it);
		}
	}
	else
	{
		for (auto& it : m_buttons)
		{
			UIMG->UIOff(it);
		}
	}
}
