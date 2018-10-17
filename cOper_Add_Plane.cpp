#include "stdafx.h"


cOper_Add_Plane::cOper_Add_Plane()
	: cOperation(OPER_ADD_PLANE)
	, m_planePos(0,0,0)
	, m_worksSate(ADD_PLANE)
{

}


cOper_Add_Plane::~cOper_Add_Plane()
{

}

void cOper_Add_Plane::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.AddParameter(L"Plane X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_planePos.x);
	m_operControl.AddParameter(L"Plane Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_planePos.y);
	m_operControl.AddParameter(L"Plane Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_planePos.z);
}

void cOper_Add_Plane::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	assert(!draw.empty() && typeid(*draw[0].get()) == typeid(cDrawPlane) &&
		"this Container don`t have DrawPlane Element");

	switch (m_worksSate)
	{
	case cOper_Add_Plane::ADD_PLANE:
		static_cast<cDrawPlane*>(draw[0].get())->AddPlanes();
		m_worksSate = cOper_Add_Plane::PLANE_ATTRIBUTE_SET;
		break;
	case cOper_Add_Plane::PLANE_ATTRIBUTE_SET:
		m_operControl.Update(XMMatrixIdentity());
		break;
	case cOper_Add_Plane::OPER_END:
		m_worksSate = cOper_Add_Plane::ADD_PLANE;
		EndOperation();
		break;
	default:
		break;
	}

	if (INPUTMG->GetMouseUp(VK_RBUTTON))
	{
		static_cast<cDrawPlane*>(draw[0].get())->DeletePlane();
		m_worksSate = cOper_Add_Plane::OPER_END;
	}
}
