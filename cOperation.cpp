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

bool cOperation::PickPlane(cDrawPlane* planes, cPlane ** plane)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		if (!m_operControl.IsMousePosInUIWindow())
		{
			cObject* pickPlane = nullptr;
			if (planes->Picking(&pickPlane))
			{
				CAMERA.SetTargetAndSettingAngle(pickPlane);
				*plane = static_cast<cPlane*>(pickPlane);
				return true;
			}
		}
	}

	return false;
}

cDot * cOperation::AddDotAtCurrPlane(DrawItems* drawItems)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		if (!m_operControl.IsMousePosInUIWindow())
		{
			float distance;
			PICKRAY ray = INPUTMG->GetMousePickLay();
			if (drawItems->m_plane->Picking(ray, distance))
			{
				cObject* pickDot = nullptr;
				if (drawItems->m_draws[DRAW_DOTS]->Picking(&pickDot))
				{
					cDot* resultDot = static_cast<cDot*>(pickDot);
					if (resultDot->GetHostObject() == drawItems->m_plane)
					{
						return resultDot;
					}
				}

				XMMATRIX planeInvMat = XMMatrixInverse(&XMVECTOR(), drawItems->m_plane->GetXMMatrix());
				XMVECTOR pos = ray.origin + ray.ray*distance;
				pos = XMVector3TransformCoord(pos, planeInvMat);
				pos.m128_f32[2] = 0;
				cDot* dot = AddDot(drawItems->m_draws);
				XMStoreFloat3(&dot->GetPos(), pos);
				dot->SetHostObject(drawItems->m_plane);

				return dot;
			}
		}
	}

	return nullptr;
}
