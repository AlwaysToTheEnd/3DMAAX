#include "stdafx.h"


cUIOperWindow::cUIOperWindow()
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

}
