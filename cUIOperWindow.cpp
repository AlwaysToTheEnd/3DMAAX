#include "stdafx.h"


cUIOperWindow::cUIOperWindow()
	: m_currParameterIndex(-1)
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
	m_renderInstance->m_isRenderOK = false;
}

void cUIOperWindow::IsRenderState(bool value)
{
	m_renderInstance->m_isRenderOK = value;

	for (int i = 0; i < m_operParameters.size(); i++)
	{
		m_operFonts[i]->isRender = value;
	}
}

void cUIOperWindow::AddParameter(wstring dataName, DXGI_FORMAT format, void * pData)
{
	m_operParameters.push_back({ dataName,format,pData });

	for (int i = m_operFonts.size(); i < m_operParameters.size(); i++)
	{
		m_operFonts.push_back(FONTMANAGER->GetFont("baseFont"));
		m_operFonts.back()->isRender = false;
	}
}

void cUIOperWindow::UIUpdate()
{
	SetSize({ 150.0f,m_operParameters.size() * 15.0f });
	SetPos({ 650,100,0 });

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

			m_currParameterIndex = inUIMouseY / 15;
		}
		else
		{
			m_currParameterIndex = -1;
		}
	}

	for (int i = 0; i < m_operParameters.size(); i++)
	{
		wstring dataStr;

		switch (m_operParameters[i].dataFormat)
		{
		case DXGI_FORMAT_D32_FLOAT:
			dataStr = to_wstring(*((float*)m_operParameters[i].data));
			break;
		case DXGI_FORMAT_R32_FLOAT:
			dataStr = to_wstring(*((float*)m_operParameters[i].data));
			break;
		case DXGI_FORMAT_R32_UINT:
			dataStr = to_wstring(*((UINT*)m_operParameters[i].data));
			break;
		case DXGI_FORMAT_R32_SINT:
			dataStr = to_wstring(*((int*)m_operParameters[i].data));
			break;
		default:
			assert(false && "It is not support this format" && m_operParameters[i].dataFormat);
			break;
		}

		if (m_currParameterIndex == i)
		{
			for (char i = '.'; i <= '9'; i++)
			{
				if (GetAsyncKeyState(i))
				{
					if (i != '/')
					{
						m_currInputData += i;
						break;
					}
				}
			}

			m_operFonts[i]->color = Colors::Red;
			m_operFonts[i]->printString = m_operParameters[i].dataName + m_currInputData;
		}
		else
		{
			m_operFonts[i]->color = Colors::Blue;
			m_operFonts[i]->printString = m_operParameters[i].dataName + dataStr;
		}

		m_operFonts[i]->pos.x = buttonRect.left;
		m_operFonts[i]->pos.y = buttonRect.top + i * 15;
		m_operFonts[i]->scale = { 0.3f,0.35f,1 };
	}
}
