#include "stdafx.h"

cObject::cObject()
	: m_renderInstance(nullptr)
{
}

cObject::~cObject()
{

}

void cObject::Build(shared_ptr<cRenderItem> renderItem)
{
	m_renderInstance = renderItem->GetRenderIsntance();
}