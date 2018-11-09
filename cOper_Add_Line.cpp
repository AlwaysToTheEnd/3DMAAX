#include "stdafx.h"


cOper_Add_Line::cOper_Add_Line()
	:cOperation(OPER_ADD_LINE)
	, m_firstDot(nullptr)
{
}

cOper_Add_Line::~cOper_Add_Line()
{

}

UINT cOper_Add_Line::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	switch (m_worksSate)
	{
	case cOper_Add_Line::CURR_DRAW_CHECK:
	{
		if (CurrDrawCheckAndPick(drawItems, currDrawItems))
		{
			m_operationText->isRender = true;
			m_firstDot = nullptr;
			m_addLineNum = 0;
			m_addDotNum = 0;
			m_operationText->printString = L"첫번째 점을 선택해주십시오";
			m_worksSate = FIRST_DOT_PICK;
		}
	}
	break;
	case cOper_Add_Line::FIRST_DOT_PICK:
	{
		m_operationText->isRender = true;
		bool isAddDot = false;
		m_firstDot = AddDotAtCurrPlane(currDrawItems, &isAddDot);

		if (m_firstDot)
		{
			if (isAddDot)
			{
				m_addDotNum++;
			}

			m_worksSate = SECEND_DOT_PICK;
			m_operationText->printString = L"두번째 점을 선택해주십시오";
		}
	}
	break;
	case cOper_Add_Line::SECEND_DOT_PICK:
	{
		bool isAddDot = false;
		cDot* secendDot = AddDotAtCurrPlane(currDrawItems, &isAddDot);

		if (secendDot&& secendDot != m_firstDot)
		{
			if (isAddDot)
			{
				m_addDotNum++;
			}

			m_addLineNum++;
			cLine* line = AddLine(currDrawItems->m_draws);
			line->SetFirstDot(m_firstDot);
			line->SetSecondDot(secendDot);
			m_firstDot = nullptr;
			m_operationText->printString = L"첫번째 점을 선택해주십시오";
			m_worksSate = FIRST_DOT_PICK;
		}
	}
	break;
	}

	return 0;
}

void cOper_Add_Line::CancleOperation(DrawItems* draw)
{
	if (draw)
	{
		for (UINT i = 0; i < m_addLineNum; i++)
		{
			draw->m_draws[DRAW_LINES]->DeleteBackObject();
		}

		for (UINT i = 0; i < m_addDotNum; i++)
		{
			draw->m_draws[DRAW_DOTS]->DeleteBackObject();
		}
	}

	EndOperation();
}

void cOper_Add_Line::EndOperation()
{
	cOperation::EndOperation();
	m_firstDot = nullptr;
	m_operationText->printString = L"첫번째 점을 선택해주십시오";
}