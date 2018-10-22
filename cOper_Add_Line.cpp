#include "stdafx.h"


cOper_Add_Line::cOper_Add_Line()
	:cOperation(OPER_ADD_LINE)
	, m_operationText(nullptr)
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

void cOper_Add_Line::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	switch (m_worksSate)
	{
	case cOper_Add_Line::SELECT_PLANE:
	{
		m_operationText->isRender = true;
		if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
		{
			cObject* pickPlane = nullptr;

			if (draw[DRAW_PLNAES]->Picking(&pickPlane))
			{
				m_currPlane = static_cast<cPlane*>(pickPlane);
				m_worksSate = FIRST_DOT_PICK;
				m_operationText->printString = L"Select First dot";
			}
		}
	}
		break;
	case cOper_Add_Line::FIRST_DOT_PICK:
	{
		m_firstDot = AddDotAtCurrPlane(draw);

		if (m_firstDot)
		{
			m_worksSate = SECEND_DOT_PICK;
			m_operationText->printString = L"Select Secend dot";
		}
	}
		break;
	case cOper_Add_Line::SECEND_DOT_PICK:
	{
		cDot* secendDot = AddDotAtCurrPlane(draw);

		if (secendDot)
		{
			cLine* line=static_cast<cLine*>(draw[DRAW_LINES]->AddElement());
			line->SetFirstDot(m_firstDot);
			line->SetSecendDot(secendDot);
			m_firstDot = nullptr;
			m_operationText->isRender=false;
			m_operationText->printString = L"Select Plane";
			EndOperation();
		}
	}
		break;
	default:
		break;
	}
}

void cOper_Add_Line::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	draw[DRAW_LINES].get()->DeleteBackObject();
	m_firstDot = nullptr;
	m_operationText->isRender = false;
	m_operationText->printString = L"Select Plane";
	EndOperation();
}

cDot * cOper_Add_Line::AddDotAtCurrPlane(vector<unique_ptr<cDrawElement>>& draw)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		float distance;
		PICKRAY ray = INPUTMG->GetMousePickLay();
		if (m_currPlane->Picking(ray, distance))
		{
			XMMATRIX planeInvMat = XMMatrixInverse(&XMVECTOR(), m_currPlane->GetXMMatrix());
			XMVECTOR pos = ray.origin + ray.ray*distance;
			pos = XMVector3TransformCoord(pos, planeInvMat);
			pos.m128_f32[2] = 0;
			cDot* dot = static_cast<cDot*>(draw[DRAW_DOTS]->AddElement());
			XMStoreFloat3(&dot->GetPos(), pos);
			dot->SetHostObject(m_currPlane);

			return dot;
		}
	}

	return nullptr;
}
