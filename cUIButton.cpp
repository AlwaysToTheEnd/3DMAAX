#include "stdafx.h"


cUIButton::cUIButton()
	: m_Param(-1)
	, m_ActiveFunc(nullptr)
{

}


cUIButton::~cUIButton()
{
}

void cUIButton::InputDataUpdate()
{
	if (PtInRect(&m_uiRECT, INPUTMG->GetMousePosPt()))
	{
		if (INPUTMG->GetMouseUp(VK_LBUTTON))
		{
			m_ActiveFunc(m_Param);
		}
	}
}

void cUIButton::UIUpdate()
{
	assert(m_Param != -1);

	m_uiRECT.left = (LONG)m_renderInstance->instanceData.World._41;
	m_uiRECT.top = (LONG)m_renderInstance->instanceData.World._42;
	m_uiRECT.right = m_uiRECT.left + (LONG)m_renderInstance->instanceData.World._11;
	m_uiRECT.bottom= m_uiRECT.top + (LONG)m_renderInstance->instanceData.World._22;
}
