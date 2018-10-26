#include "stdafx.h"


cOper_Add_Line::cOper_Add_Line()
	:cOperation(OPER_ADD_LINE)
	, m_firstDot(nullptr)
{
}

cOper_Add_Line::~cOper_Add_Line()
{

}

void cOper_Add_Line::SetUp()
{
	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Select First Dot";
	m_operationText->pos = { 30,30,0 };
}

void cOper_Add_Line::DrawElementOperation(DrawItems* drawItems)
{
	switch (m_worksSate)
	{
	case cOper_Add_Line::FIRST_DOT_PICK:
	{
		m_operationText->isRender = true;
		m_firstDot = AddDotAtCurrPlane(drawItems);

		if (m_firstDot)
		{
			m_worksSate = SECEND_DOT_PICK;
			m_operationText->printString = L"Select Secend dot";
		}
	}
	break;
	case cOper_Add_Line::SECEND_DOT_PICK:
	{
		cDot* secendDot = AddDotAtCurrPlane(drawItems);

		if (secendDot)
		{
			cLine* line = AddLine(drawItems->m_draws);
			line->SetFirstDot(m_firstDot);
			line->SetSecondDot(secendDot);
			m_firstDot = nullptr;
			m_operationText->printString = L"Select First Dot";
			EndOperation();
		}
	}
	break;
	}
}

void cOper_Add_Line::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	draw[DRAW_LINES]->DeleteBackObject();

	if (m_firstDot)
	{
		draw[DRAW_DOTS]->DeleteBackObject();
	}

	m_firstDot = nullptr;
	EndOperation();
}

void cOper_Add_Line::EndOperation()
{
	cOperation::EndOperation();
	m_firstDot = nullptr;
	m_operationText->printString = L"Select First Dot";
}