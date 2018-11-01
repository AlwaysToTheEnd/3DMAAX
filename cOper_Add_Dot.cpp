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
	m_operControl.SetPos({ 650,100,0 });
	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Set dot Time";
	m_operationText->pos = { 30,30,0 };
}

void cOper_Add_Dot::DrawElementOperation(DrawItems* drawItems)
{
	switch (m_worksSate)
	{
	case cOper_Add_Dot::SET_DOTS:
	{
		cDot* addDot = AddDotAtCurrPlane(drawItems);
		m_operationText->isRender = true;

		if (addDot != nullptr && m_currDot != addDot)
		{
			if (m_currDot)
			{
				m_currDot->SetPickRender(2);
			}

			m_currDot = addDot;
			m_operControl.ClearParameters();
			m_operControl.AddParameter(L"pos    X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currDot->GetPos().x);
			m_operControl.AddParameter(L"pos    Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currDot->GetPos().y);
			m_operControl.SetRenderState(1);

			m_currDot->SetPickRender(1);
		}

		if (m_existingDotNum < drawItems->m_draws[DRAW_DOTS]->GetObjectNum())
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
	EndOperation();

	for (UINT i = 0; i < m_addDotCount; i++)
	{
		draw[DRAW_DOTS]->DeleteBackObject();
	}
}

void cOper_Add_Dot::EndOperation()
{
	cOperation::EndOperation();

	if (m_currDot)
	{
		m_currDot->SetPickRender(2);
	}

	m_currDot = nullptr;
	m_addDotCount = 0;
}
