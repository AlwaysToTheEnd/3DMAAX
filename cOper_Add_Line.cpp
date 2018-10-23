#include "stdafx.h"


cOper_Add_Line::cOper_Add_Line()
	:cOperation(OPER_ADD_LINE)
	, m_currPlane(nullptr)
	, m_firstDot(nullptr)
{
}

cOper_Add_Line::~cOper_Add_Line()
{

}

void cOper_Add_Line::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Select Plane";
	m_operationText->pos = { 30,30,0 };
}

void cOper_Add_Line::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw, cDrawPlane* planes)
{
	switch (m_worksSate)
	{
	case cOper_Add_Line::SELECT_PLANE:
	{
		m_operationText->isRender = true;

		if (PickPlane(draw, &m_currPlane))
		{
			m_worksSate = FIRST_DOT_PICK;
			m_operationText->printString = L"Select First dot";

			CAMERA.SetTargetAndSettingAngle(m_currPlane);
		}
	}
	break;
	case cOper_Add_Line::FIRST_DOT_PICK:
	{
		m_firstDot = AddDotAtCurrPlane(draw, m_currPlane);

		if (m_firstDot)
		{
			m_worksSate = SECEND_DOT_PICK;
			m_operationText->printString = L"Select Secend dot";
		}
	}
	break;
	case cOper_Add_Line::SECEND_DOT_PICK:
	{
		cDot* secendDot = AddDotAtCurrPlane(draw,m_currPlane);

		if (secendDot)
		{
			cLine* line = AddLine(draw);
			line->SetFirstDot(m_firstDot);
			line->SetSecendDot(secendDot);
			m_firstDot = nullptr;
			m_operationText->isRender = false;
			m_operationText->printString = L"Select Plane";
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
	m_operationText->printString = L"Select Plane";
	EndOperation();
}

void cOper_Add_Line::EndOperation()
{
	cOperation::EndOperation();
	m_firstDot = nullptr;
	m_operationText->printString = L"Select Plane";
}