#include "stdafx.h"

shared_ptr<cRenderItem> cOperation::m_OperatorUi = nullptr;
shared_ptr<cRenderItem> cOperation::m_prevViewRender = nullptr;
shared_ptr<RenderInstance> cOperation::m_prevViewRenderInstance = nullptr;
MeshGeometry* cOperation::m_previewGeo = nullptr;

void cOperation::OperationsBaseSetup(shared_ptr<cRenderItem> renderItem)
{
	m_OperatorUi = renderItem;
	cUIObject::SetGeoAtRenderItem(renderItem);

	m_previewGeo = MESHMG->AddTemporaryMesh("previewGeo");
	m_prevViewRender = RENDERITEMMG->AddRenderItem(cDrawMesh::m_meshRenderName);
	m_prevViewRender->SetGeometry(m_previewGeo, m_previewGeo->name);
	m_prevViewRenderInstance = m_prevViewRender->GetRenderIsntance();
	m_prevViewRenderInstance->m_isRenderOK = false;
	m_prevViewRender->SetRenderOK(false);
}

void cOperation::EndOperation()
{
	m_operState = false; 
	m_operControl.SetRenderState(false); 
	m_operControl.ClearParameters();
	m_prevViewRender->SetRenderOK(false);
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
					return resultDot;
				}

				XMMATRIX planeInvMat = XMMatrixInverse(&XMVECTOR(), drawItems->m_plane->GetXMMatrix());
				XMVECTOR pos = ray.origin + ray.ray*distance;
				pos = XMVector3TransformCoord(pos, planeInvMat);
				pos.m128_f32[2] = 0;
				cDot* dot = AddDot(drawItems->m_draws);
				dot->SetHostObject(drawItems->m_plane);
				XMStoreFloat3(&dot->GetPos(), pos);

				return dot;
			}
		}
	}

	return nullptr;
}
