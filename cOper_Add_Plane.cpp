#include "stdafx.h"


cOper_Add_Plane::cOper_Add_Plane()
	: cOperation(OPER_ADD_PLANE)
{

}


cOper_Add_Plane::~cOper_Add_Plane()
{

}

void cOper_Add_Plane::SetUp()
{
	m_operControl.Build(m_OperatorUi);
}

void cOper_Add_Plane::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	assert(!draw.empty() && typeid(*draw[DRAW_PLNAES].get()) == typeid(cDrawPlane) &&
		"this Container didn`t have DrawPlane Element at first slot");

	switch (m_worksSate)
	{
	case cOper_Add_Plane::ADD_PLANE:
	{
		cObject* element = draw[DRAW_PLNAES].get()->AddElement();
		OBJCOORD->ObjectRegistration(element);
		m_operControl.AddParameter(L"quater X : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetQuaternion().x);
		m_operControl.AddParameter(L"quater Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetQuaternion().y);
		m_operControl.AddParameter(L"quater Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetQuaternion().z);
		m_operControl.AddParameter(L"quater W : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetQuaternion().w);
		m_operControl.AddParameter(L"pos    X : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetPos().x);
		m_operControl.AddParameter(L"pos    Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetPos().y);
		m_operControl.AddParameter(L"pos    Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&element->GetPos().z);
		m_operControl.IsRenderState(true);
		m_worksSate = cOper_Add_Plane::PLANE_ATTRIBUTE_SET;
	}
	break;
	case cOper_Add_Plane::PLANE_ATTRIBUTE_SET:
		m_operControl.Update(XMMatrixIdentity());
		break;
	default:
		break;
	}
}

void cOper_Add_Plane::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	draw[DRAW_PLNAES].get()->DeleteBackObject();
	EndOperation();
}
