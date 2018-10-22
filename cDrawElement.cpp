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

bool cDrawElement::Picking(cObject ** ppObject)
{
	float dist = FLT_MAX;
	PICKRAY mouseRay = INPUTMG->GetMousePickLay();

	for (auto& it : m_objects)
	{
		float currObjectDist;
		if (it->Picking(mouseRay, currObjectDist))
		{
			if (currObjectDist < dist)
			{
				*ppObject = it.get();
				dist = currObjectDist;
			}
		}
	}

	return dist != FLT_MAX;
}

void cDrawElement::DeleteBackObject()
{
	if (m_objects.size())
	{
		m_objects.pop_back();
	}
}
