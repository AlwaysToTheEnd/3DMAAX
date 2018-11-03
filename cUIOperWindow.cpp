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
	assert(format == OPERDATATYPE_FUNC_INT_PARAM);

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

bool cUIOperWindow::IsMousePosInUIWindow()
{
	if (m_renderInstance == nullptr) return false;

	RECT buttonRect;
	buttonRect.left = (LONG)m_renderInstance->instanceData.World._41;
	buttonRect.top = (LONG)m_renderInstance->instanceData.World._42;
	buttonRect.right = buttonRect.left + (LONG)m_renderInstance->instanceData.World._11;
	buttonRect.bottom = buttonRect.top + (LONG)m_renderInstance->instanceData.World._22;
	POINT mousePos = INPUTMG->GetMousePosPt();

	return PtInRect(&buttonRect, mousePos);
}

void cUIOperWindow::UIUpdate()
{
	SetSize({ 150.0f,m_operParameters.size() * 15.0f });

	RECT buttonRect;
	buttonRect.left = (LONG)m_renderInstance->instanceData.World._41;
	buttonRect.top = (LONG)m_renderInstance->instanceData.World._42;
	buttonRect.right = buttonRect.left + (LONG)m_renderInstance->instanceData.World._11;
	buttonRect.bottom = buttonRect.top + (LONG)m_renderInstance->instanceData.World._22;

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
			m_currInputData.clear();
		}
	}

	if (m_currParameterIndex != -1)
	{
		KeyboardDataInput();
		EnterAction();
	}

	for (int i = 0; i < m_operParameters.size(); i++)
	{
		if (i == m_currParameterIndex)
		{
			m_operFonts[i]->color = Colors::Red;
			m_operFonts[i]->printString = m_operParameters[i].dataName;
			m_operFonts[i]->pos.x = (float)buttonRect.left;
			m_operFonts[i]->pos.y = (float)buttonRect.top + i * 15;
			m_operFonts[i]->scale = { 0.3f,0.35f,1 };

			switch (m_operParameters[i].dataFormat)
			{
			case OPERDATATYPE_INDEX:
				break;
			case OPERDATATYPE_BOOL:
			{
				*(bool*)(m_operParameters[i].data) = !(*(bool*)(m_operParameters[i].data));
				m_currParameterIndex = -1;
				if (*(bool*)(m_operParameters[i].data) == true)
				{
					m_operFonts[i]->color = Colors::Red;
				}
				else
				{
					m_operFonts[i]->color = Colors::Black;
				}

				m_operFonts[i]->printString = m_operParameters[i].dataName;
			}
				break;
			case OPERDATATYPE_FLOAT:
				m_operFonts[i]->printString = m_operParameters[i].dataName + m_currInputData;
				break;
			case OPERDATATYPE_FUNC:
				break;
			case OPERDATATYPE_FUNC_INT_PARAM:
				break;
			case OPERDATATYPE_NONE:
				break;
			}
		}
		else
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
			case OPERDATATYPE_FUNC_INT_PARAM:
				break;
			case OPERDATATYPE_NONE:
				break;
			}

			m_operFonts[i]->color = Colors::Black;
			m_operFonts[i]->printString = m_operParameters[i].dataName + dataStr;
			m_operFonts[i]->pos.x = (float)buttonRect.left;
			m_operFonts[i]->pos.y = (float)buttonRect.top + i * 15;
			m_operFonts[i]->scale = { 0.3f,0.35f,1 };
		}
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
			function<void()> func = reinterpret_cast<void(*)()>(m_operParameters[m_currParameterIndex].data);
			func();
		}
		break;
		case OPERDATATYPE_FUNC_INT_PARAM:
		{
			function<void(int)> func = reinterpret_cast<void(*)(int)>(m_operParameters[m_currParameterIndex].data);
			func(m_operParameters[m_currParameterIndex].funcParam);
		}
		break;
		default:
			break;
		}

		m_currInputData.clear();
		m_currParameterIndex = -1;
	}
}
