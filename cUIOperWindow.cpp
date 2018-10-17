#include "stdafx.h"


cUIOperWindow::cUIOperWindow()
	: m_currParameterIndex(-1)
	, m_renderItem(nullptr)
	, m_fontSize(10)
{
}


cUIOperWindow::~cUIOperWindow()
{

}

void cUIOperWindow::Build(shared_ptr<cRenderItem> renderItem)
{
	m_renderInstance = renderItem->GetRenderIsntance();
	m_renderInstance->instanceData.MaterialIndex = 0;
	m_renderItem = renderItem;
}

void cUIOperWindow::AddParameter(string dataName, DXGI_FORMAT format, void * pData)
{
	m_operParameters.push_back({ dataName,format,pData });
	m_operFonts.push_back(FONTMANAGER->GetFont("baseFont"));
}

void cUIOperWindow::UIUpdate()
{
	RECT buttonRect;
	buttonRect.left = m_renderInstance->instanceData.World._41;
	buttonRect.top = m_renderInstance->instanceData.World._42;
	buttonRect.right = buttonRect.left + m_renderInstance->instanceData.World._11;
	buttonRect.bottom = buttonRect.top + m_renderInstance->instanceData.World._22;

	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		POINT mousePos = INPUTMG->GetMousePosPt();
		if (PtInRect(&buttonRect, mousePos))
		{
			int inUIMouseY = mousePos.y - buttonRect.top;


		}
	}

	for (int i = 0; i < m_operFonts.size(); i++)
	{
		
	}
}
