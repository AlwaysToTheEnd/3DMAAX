#include "stdafx.h"


cOper_Add_Line::cOper_Add_Line()
	:cOperation(OPER_ADD_LINE)
	, m_operationText(nullptr)
	, m_currPlane(nullptr)
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
}

void cOper_Add_Line::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	switch (m_worksSate)
	{
	case cOper_Add_Line::SELECT_PLANE:
		if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
		{
			cObject* pickPlane = nullptr;

			if (draw[DRAW_PLNAES].get()->Picking(&pickPlane))
			{
				m_currPlane = static_cast<cPlane*>(pickPlane);
				m_worksSate = FIRST_DOT_PICK;
			}
		}
		break;
	case cOper_Add_Line::FIRST_DOT_PICK:
		break;
	case cOper_Add_Line::SECEND_DOT_PICK:
		break;
	case cOper_Add_Line::OPER_END:
		break;
	default:
		break;
	}
}

void cOper_Add_Line::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	draw[DRAW_LINES].get()->DeleteBackObject();
	EndOperation();
}
