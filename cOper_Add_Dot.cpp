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


UINT cOper_Add_Dot::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	switch (m_worksSate)
	{
	case cOper_Add_Dot::CURR_DRAW_CHECK:

		if (CurrDrawCheckAndPick(drawItems, currDrawItems))
		{
			m_worksSate = SET_DOTS;
			m_operationText->isRender = true;
			m_operationText->printString = L"set dot time";
		}

		break;
	case cOper_Add_Dot::SET_DOTS:
	{
		cDot* addDot = AddDotAtCurrPlane(currDrawItems);
		m_operationText->isRender = true;

		if (addDot != nullptr && m_currDot != addDot)
		{
			if (m_currDot)
			{
				m_currDot->SetPickRender(2);
			}

			m_currDot = addDot;
			m_operControl->ClearParameters();
			m_operControl->AddParameter(L"pos    X : ", OPERDATATYPE_FLOAT, (void*)&m_currDot->GetPos().x);
			m_operControl->AddParameter(L"pos    Y : ", OPERDATATYPE_FLOAT, (void*)&m_currDot->GetPos().y);
			UIMG->UIOn(m_operControl);

			m_currDot->SetPickRender(1);
		}

		if (m_existingDotNum < currDrawItems->m_draws[DRAW_DOTS]->GetObjectNum())
		{
			m_existingDotNum++;
			m_addDotCount++;
		}

	}
		break;
	default:
		break;
	}

	return 0;
}

void cOper_Add_Dot::CancleOperation(DrawItems* draw)
{
	EndOperation();

	for (UINT i = 0; i < m_addDotCount; i++)
	{
		draw->m_draws[DRAW_DOTS]->DeleteBackObject();
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
