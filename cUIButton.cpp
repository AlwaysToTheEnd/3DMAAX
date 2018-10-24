#include "stdafx.h"


cUIButton::cUIButton()
	: m_Param(-1)
	, m_ActiveFunc(nullptr)
{

}


cUIButton::~cUIButton()
{
}

void cUIButton::UIUpdate()
{
	assert(m_Param != -1);

	RECT buttonRect;
	buttonRect.left = (LONG)m_renderInstance->instanceData.World._41;
	buttonRect.top = (LONG)m_renderInstance->instanceData.World._42;
	buttonRect.right = buttonRect.left + (LONG)m_renderInstance->instanceData.World._11;
	buttonRect.bottom= buttonRect.top + (LONG)m_renderInstance->instanceData.World._22;

	if (PtInRect(&buttonRect, INPUTMG->GetMousePosPt()))
	{
		if (INPUTMG->GetMouseUp(VK_LBUTTON))
		{
			m_ActiveFunc(m_Param);
		}
	}
}
