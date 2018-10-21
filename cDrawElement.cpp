#include "stdafx.h"


cDrawElement::cDrawElement()
	:m_renderItem(nullptr)
{
}


cDrawElement::~cDrawElement()
{
}

void cDrawElement::Update()
{
	for (auto& it : m_objects)
	{
		it->Update(XMMatrixIdentity());
	}
}

void cDrawElement::DeleteBackObject()
{
	if (m_objects.size())
	{
		m_objects.pop_back();
	}
}
