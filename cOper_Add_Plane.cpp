#include "stdafx.h"


cOper_Add_Plane::cOper_Add_Plane()
	: cOperation(OPER_ADD_PLANE)
	, m_planeNormal(0,0,1)
	, m_worksSate(ADD_PLANE)
{

}


cOper_Add_Plane::~cOper_Add_Plane()
{

}

void cOper_Add_Plane::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.AddParameter(L"normal X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_planeNormal.x);
	m_operControl.AddParameter(L"normal Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_planeNormal.y);
	m_operControl.AddParameter(L"normal Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_planeNormal.z);
	m_operControl.AddParameter(L"distance : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_distance);
}

void cOper_Add_Plane::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	assert(!draw.empty() && typeid(*draw[0].get()) == typeid(cDrawPlane) &&
		"this Container don`t had DrawPlane Element at first slot");

	switch (m_worksSate)
	{
	case cOper_Add_Plane::ADD_PLANE:
		static_cast<cDrawPlane*>(draw[0].get())->AddPlanes();
		m_worksSate = cOper_Add_Plane::PLANE_ATTRIBUTE_SET;
		break;
	case cOper_Add_Plane::PLANE_ATTRIBUTE_SET:
	{
		XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&m_planeNormal));
		static_cast<cDrawPlane*>(draw[0].get())->SetPlaneInfo(
			XMPlaneFromPointNormal(m_distance*normal , normal));
		m_operControl.Update(XMMatrixIdentity());
	}
		break;
	case cOper_Add_Plane::OPER_END:
		m_worksSate = cOper_Add_Plane::ADD_PLANE;
		EndOperation();
		break;
	default:
		break;
	}

	if (GetAsyncKeyState(VK_SPACE)&0x0001)
	{
		m_worksSate = cOper_Add_Plane::OPER_END;
	}

	if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
	{
		static_cast<cDrawPlane*>(draw[0].get())->DeletePlane();
		m_worksSate = cOper_Add_Plane::OPER_END;
	}
}
