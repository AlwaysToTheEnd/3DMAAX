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

	if (m_operationText)
	{
		m_operationText->isRender = false;
	}
	m_worksSate = 0;
}

cDot * cOperation::AddDot(vector<unique_ptr<cDrawElement>>& draw)
{
	return static_cast<cDot*>(draw[DRAW_DOTS]->AddElement());
}

cLine* cOperation::AddLine(vector<unique_ptr<cDrawElement>>& draw)
{
	return static_cast<cLine*>(draw[DRAW_LINES]->AddElement());
}

bool cOperation::PickPlane(vector<unique_ptr<cDrawElement>>& draw, cPlane ** plane)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		cObject* pickPlane = nullptr;
		if (draw[DRAW_PLNAES]->Picking(&pickPlane))
		{
			*plane = static_cast<cPlane*>(pickPlane);
			return true;
		}
	}

	return false;
}

cDot * cOperation::AddDotAtCurrPlane(vector<unique_ptr<cDrawElement>>& draw, cPlane * currPlane)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		float distance;
		PICKRAY ray = INPUTMG->GetMousePickLay();
		if (currPlane->Picking(ray, distance))
		{
			cObject* pickDot = nullptr;
			if (draw[DRAW_DOTS]->Picking(&pickDot))
			{
				cDot* resultDot = static_cast<cDot*>(pickDot);
				if (resultDot->GetHostObject() == currPlane)
				{
					return resultDot;
				}
			}

			XMMATRIX planeInvMat = XMMatrixInverse(&XMVECTOR(), currPlane->GetXMMatrix());
			XMVECTOR pos = ray.origin + ray.ray*distance;
			pos = XMVector3TransformCoord(pos, planeInvMat);
			pos.m128_f32[2] = 0;
			cDot* dot = AddDot(draw);
			XMStoreFloat3(&dot->GetPos(), pos);
			dot->SetHostObject(currPlane);

			return dot;
		}
	}

	return nullptr;
}
