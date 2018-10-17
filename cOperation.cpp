#include "stdafx.h"

shared_ptr<cRenderItem> cOperation::m_OperatorUi = nullptr;

void cOperation::SetOperatorUIRender(shared_ptr<cRenderItem> renderItem)
{
	m_OperatorUi = renderItem;
	cUIObject::SetGeoAtRenderItem(renderItem);
}
