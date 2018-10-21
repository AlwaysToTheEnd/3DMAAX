#include "stdafx.h"

shared_ptr<cRenderItem> cOperation::m_OperatorUi = nullptr;

void cOperation::SetOperatorUIRender(shared_ptr<cRenderItem> renderItem)
{
	m_OperatorUi = renderItem;
	cUIObject::SetGeoAtRenderItem(renderItem);
}

void cOperation::EndOperation()
{
	m_operState = false; 
	m_operControl.IsRenderState(false); 
	m_operControl.ClearParameters();
	OBJCOORD->ObjectRegistration(nullptr);
	m_worksSate = 0;
}
