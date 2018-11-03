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
			m_operationText->printString = L"Select First Dot";
			m_worksSate = FIRST_DOT_PICK;
		}
	}
	break;
	case cOper_Add_Line::FIRST_DOT_PICK:
	{
		m_operationText->isRender = true;
		m_firstDot = AddDotAtCurrPlane(currDrawItems);

		if (m_firstDot)
		{
			m_worksSate = SECEND_DOT_PICK;
			m_operationText->printString = L"Select Secend dot";
		}
	}
	break;
	case cOper_Add_Line::SECEND_DOT_PICK:
	{
		cDot* secendDot = AddDotAtCurrPlane(currDrawItems);

		if (secendDot&& secendDot != m_firstDot)
		{
			cLine* line = AddLine(currDrawItems->m_draws);
			line->SetFirstDot(m_firstDot);
			line->SetSecondDot(secendDot);
			m_firstDot = nullptr;
			m_operationText->printString = L"Select First Dot";
			EndOperation();
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
		draw->m_draws[DRAW_LINES]->DeleteBackObject();

		if (m_firstDot)
		{
			draw->m_draws[DRAW_DOTS]->DeleteBackObject();
		}

		m_firstDot = nullptr;
	}

	EndOperation();
}

void cOper_Add_Line::EndOperation()
{
	cOperation::EndOperation();
	m_firstDot = nullptr;
	m_operationText->printString = L"Select First Dot";
}