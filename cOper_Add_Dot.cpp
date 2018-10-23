#include "stdafx.h"


cOper_Add_Dot::cOper_Add_Dot()
	: cOperation(OPER_ADD_DOT)
	, m_currPlane(nullptr)
	, m_currDot(nullptr)
	, m_addDotCount(0)
	, m_existingDotNum(0)
{

}


cOper_Add_Dot::~cOper_Add_Dot()
{
}

void cOper_Add_Dot::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Select Plane";
	m_operationText->pos = { 30,30,0 };
}

void cOper_Add_Dot::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	switch (m_worksSate)
	{
	case cOper_Add_Dot::SELECT_PLANE:
	{
		m_operationText->isRender = true;
		m_existingDotNum = draw[DRAW_DOTS]->GetObjectNum();

		if (PickPlane(draw, &m_currPlane))
		{
			m_worksSate = SET_DOTS;
			CAMERA.SetTargetAndSettingAngle(m_currPlane);
			m_operationText->printString = L"Set Dot Time!";
		}
	}
		break;
	case cOper_Add_Dot::SET_DOTS:
	{
		cDot* addDot = AddDotAtCurrPlane(draw, m_currPlane);

		if (addDot != nullptr && m_currDot != addDot)
		{
			if (m_currDot)
			{
				m_currDot->SetPickRender(0);
			}

			m_currDot = addDot;
			m_operControl.ClearParameters();
			m_operControl.AddParameter(L"pos    X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currDot->GetPos().x);
			m_operControl.AddParameter(L"pos    Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currDot->GetPos().y);
			m_operControl.IsRenderState(1);

			m_currDot->SetPickRender(0);
		}

		if (m_existingDotNum < draw[DRAW_DOTS]->GetObjectNum())
		{
			m_existingDotNum++;
			m_addDotCount++;
		}

		m_operControl.Update(XMMatrixIdentity());
	}
		break;
	default:
		break;
	}
}

void cOper_Add_Dot::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	for (UINT i = 0; i < m_addDotCount; i++)
	{
		draw[DRAW_DOTS]->DeleteBackObject();
	}

	EndOperation();
}

void cOper_Add_Dot::EndOperation()
{
	cOperation::EndOperation();

	if (m_currDot)
	{
		m_currDot->SetPickRender(false);
	}

	m_currDot = nullptr;
	m_addDotCount = 0;
}
