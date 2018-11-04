#include "stdafx.h"


cUIButton::cUIButton()
	: m_Param(-1)
	, m_ActiveFunc(nullptr)
	, m_buttonRect{0,0,0,0}
{

}


cUIButton::~cUIButton()
{
}

bool cUIButton::IsMousePosInUI()
{
	return PtInRect(&m_buttonRect, INPUTMG->GetMousePosPt());
}

void cUIButton::UIUpdate()
{
	assert(m_Param != -1);

	m_buttonRect.left = (LONG)m_renderInstance->instanceData.World._41;
	m_buttonRect.top = (LONG)m_renderInstance->instanceData.World._42;
	m_buttonRect.right = m_buttonRect.left + (LONG)m_renderInstance->instanceData.World._11;
	m_buttonRect.bottom= m_buttonRect.top + (LONG)m_renderInstance->instanceData.World._22;

	if (PtInRect(&m_buttonRect, INPUTMG->GetMousePosPt()))
	{
		if (INPUTMG->GetMouseUp(VK_LBUTTON))
		{
			m_ActiveFunc(m_Param);
		}
	}
}
