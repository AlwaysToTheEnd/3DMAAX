#include "stdafx.h"


cUIOperWindow::cUIOperWindow()
	: m_currParameterIndex(-1)
	, m_fontSize(10)
{
}


cUIOperWindow::~cUIOperWindow()
{

}

void cUIOperWindow::SetRenderState(bool value)
{
	if (m_renderInstance)
	{
		m_renderInstance->m_isRenderOK = value;

		for (int i = 0; i < m_operParameters.size(); i++)
		{
			m_operFonts[i]->isRender = value;
		}
	}
}

void cUIOperWindow::AddParameter(wstring dataName, UIOPERDATATYPE format, void * pData)
{
	m_operParameters.emplace_back(dataName, format, nullptr, nullptr, pData, 0);
	ButtonSet();
}

void cUIOperWindow::AddParameter(wstring dataName, UIOPERDATATYPE format, function<void()> func)
{
	assert(format == OPERDATATYPE_FUNC);

	m_operParameters.emplace_back(dataName, format, func, nullptr, nullptr, 0);
	ButtonSet();
}

void cUIOperWindow::AddParameter(wstring dataName, UIOPERDATATYPE format, function<void(UINT64)> func, UINT64 param)
{
	assert(format == OPERDATATYPE_FUNC_UINT_PARAM);

	m_operParameters.emplace_back(dataName, format, nullptr, func, nullptr, param);
	ButtonSet();
}

void cUIOperWindow::ClearParameters()
{
	m_operParameters.clear();
	for (auto& it : m_operFonts)
	{
		it->isRender = false;
	}

	m_currParameterIndex = -1;
}

void cUIOperWindow::InputDataUpdate()
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		POINT mousePos = INPUTMG->GetMousePosPt();
		if (PtInRect(&m_uiRECT, mousePos))
		{
			int inUIMouseY = mousePos.y - m_uiRECT.top;

			m_currParameterIndex = (int)(inUIMouseY / 20.0f);
		}
		else
		{
			m_currParameterIndex = -1;
			m_currInputData.clear();
		}
	}

	if (m_currParameterIndex != -1)
	{
		KeyboardDataInput();
		EnterAction();

		if (m_currParameterIndex != -1)
		{
			m_operFonts[m_currParameterIndex]->color = Colors::Red;
			m_operFonts[m_currParameterIndex]->printString = m_operParameters[m_currParameterIndex].dataName;
			//m_operFonts[m_currParameterIndex]->pos.x = (float)m_uiRECT.left + 1.0f;
			//m_operFonts[m_currParameterIndex]->pos.y = (float)m_uiRECT.top + m_currParameterIndex * 20.0f + 2.0f;
			//m_operFonts[m_currParameterIndex]->scale = { 0.45f,0.45f,1 };

			switch (m_operParameters[m_currParameterIndex].dataFormat)
			{
			case OPERDATATYPE_INDEX:
				break;
			case OPERDATATYPE_BOOL:
			{
				*(bool*)(m_operParameters[m_currParameterIndex].data) = !(*(bool*)(m_operParameters[m_currParameterIndex].data));
				if (*(bool*)(m_operParameters[m_currParameterIndex].data) == true)
				{
					m_operFonts[m_currParameterIndex]->color = Colors::Red;
				}
				else
				{
					m_operFonts[m_currParameterIndex]->color = Colors::Black;
				}

				m_operFonts[m_currParameterIndex]->printString = m_operParameters[m_currParameterIndex].dataName;
				m_currParameterIndex = -1;
			}
			break;
			case OPERDATATYPE_FLOAT:
				m_operFonts[m_currParameterIndex]->printString = m_operParameters[m_currParameterIndex].dataName + m_currInputData;
				break;
			case OPERDATATYPE_FUNC:
				break;
			case OPERDATATYPE_FUNC_UINT_PARAM:
				break;
			case OPERDATATYPE_NONE:
				m_operFonts[m_currParameterIndex]->color = Colors::Black;
				break;
			}
		}
	}
}

void cUIOperWindow::UIUpdate()
{
	SetSize({ 200.0f,m_operParameters.size() * 20.0f });

	m_uiRECT.left = (LONG)m_renderInstance->instanceData.World._41;
	m_uiRECT.top = (LONG)m_renderInstance->instanceData.World._42;
	m_uiRECT.right = m_uiRECT.left + (LONG)m_renderInstance->instanceData.World._11;
	m_uiRECT.bottom = m_uiRECT.top + (LONG)m_renderInstance->instanceData.World._22;

	for (int i = 0; i < m_operParameters.size(); i++)
	{
		wstring dataStr;

		switch (m_operParameters[i].dataFormat)
		{
		case OPERDATATYPE_INDEX:
			break;
		case OPERDATATYPE_BOOL:
			break;
		case OPERDATATYPE_FLOAT:
			dataStr = to_wstring(*((float*)m_operParameters[i].data));
			break;
		case OPERDATATYPE_FUNC:
			break;
		case OPERDATATYPE_FUNC_UINT_PARAM:
			break;
		case OPERDATATYPE_NONE:
			break;
		}

		m_operFonts[i]->color = Colors::Black;
		m_operFonts[i]->printString = m_operParameters[i].dataName + dataStr;
		m_operFonts[i]->pos.x = (float)m_uiRECT.left + 1.0f;
		m_operFonts[i]->pos.y = (float)m_uiRECT.top + i * 20.0f + 2.0f;
		m_operFonts[i]->scale = { 0.45f,0.45f,1 };
	}
}

void cUIOperWindow::ButtonSet()
{
	for (size_t i = 0; i < m_operParameters.size(); i++)
	{
		if (i >= m_operFonts.size())
		{
			m_operFonts.push_back(FONTMANAGER->GetFont("baseFont"));
		}

		m_operFonts[i]->isRender = m_renderInstance->m_isRenderOK;
	}
}

void cUIOperWindow::KeyboardDataInput()
{
	for (char i = '0'; i <= '9'; i++)
	{
		if (GetAsyncKeyState(i) & 0x0001)
		{
			m_currInputData += i;
			break;
		}
	}

	if (GetAsyncKeyState(VK_OEM_MINUS) & 0x0001)
	{
		if (m_currInputData.size() == 0)
		{
			m_currInputData += '-';
		}
	}

	if (GetAsyncKeyState(VK_OEM_PERIOD) & 0x0001)
	{
		m_currInputData += '.';
	}

	if (GetAsyncKeyState(VK_BACK) & 0x0001)
	{
		if (m_currInputData.size())
		{
			m_currInputData.pop_back();
		}
	}
}

void cUIOperWindow::EnterAction()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		switch (m_operParameters[m_currParameterIndex].dataFormat)
		{
		case OPERDATATYPE_INDEX:
			*(int*)(m_operParameters[m_currParameterIndex].data) = m_currParameterIndex;
			break;
		case OPERDATATYPE_BOOL:
			break;
		case OPERDATATYPE_FLOAT:
			*(float*)(m_operParameters[m_currParameterIndex].data) = (float)_wtof(m_currInputData.c_str());
			break;
		case OPERDATATYPE_FUNC:
		{
			m_operParameters[m_currParameterIndex].func();
		}
		break;
		case OPERDATATYPE_FUNC_UINT_PARAM:
		{
			m_operParameters[m_currParameterIndex].param_Func(m_operParameters[m_currParameterIndex].funcParam);
		}
		break;
		default:
			break;
		}

		m_currInputData.clear();
		m_currParameterIndex = -1;
	}
}
