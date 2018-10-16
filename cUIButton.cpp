#include "stdafx.h"


cUIButton::cUIButton()
	: m_Param(0)
	, m_ActiveFunc(nullptr)
{

}


cUIButton::~cUIButton()
{
}

void cUIButton::UIUpdate()
{
	RECT buttonRect;
	buttonRect.left = m_renderInstance->instanceData.World._41;
	buttonRect.top = m_renderInstance->instanceData.World._42;
	buttonRect.right = buttonRect.left + m_renderInstance->instanceData.World._11;
	buttonRect.bottom= buttonRect.top + m_renderInstance->instanceData.World._22;

	if (PtInRect(&buttonRect, INPUTMG->GetMousePosPt()))
	{
		if (INPUTMG->GetMouseUp(VK_LBUTTON))
		{
			m_ActiveFunc(m_Param);
		}
	}
}
