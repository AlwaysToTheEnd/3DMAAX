#include "stdafx.h"


cOper_Add_Plane::cOper_Add_Plane()
	: cOperation(OPER_ADD_PLANE)
	, m_rotation(0,0,0)
	, m_position(0,0,0)
	, m_scale(1,1,1)
	, m_worksSate(ADD_PLANE)
{

}


cOper_Add_Plane::~cOper_Add_Plane()
{

}

void cOper_Add_Plane::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.AddParameter(L"rot    X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_rotation.x);
	m_operControl.AddParameter(L"rot    Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_rotation.y);
	m_operControl.AddParameter(L"rot    Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_rotation.z);
	m_operControl.AddParameter(L"pos    X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_position.x);
	m_operControl.AddParameter(L"pos    Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_position.y);
	m_operControl.AddParameter(L"pos    Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_position.z);
}

void cOper_Add_Plane::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	assert(!draw.empty() && typeid(*draw[DRAW_PLNAES].get()) == typeid(cDrawPlane) &&
		"this Container didn`t have DrawPlane Element at first slot");
	switch (m_worksSate)
	{
	case cOper_Add_Plane::ADD_PLANE:
		OBJCOORD->ObjectRegistration(static_cast<cDrawPlane*>(draw[0].get())->AddElement());
		m_worksSate = cOper_Add_Plane::PLANE_ATTRIBUTE_SET;
		break;
	case cOper_Add_Plane::PLANE_ATTRIBUTE_SET:
	{
	/*	XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&m_planeNormal));
		static_cast<cDrawPlane*>(draw[0].get())->SetPlaneInfo(
			XMPlaneFromPointNormal(XMLoadFloat3(&m_position) , normal));*/
		m_operControl.Update(XMMatrixIdentity());
	}
		break;
	case cOper_Add_Plane::OPER_END:
		EndOperation();
		OBJCOORD->ObjectRegistration(nullptr);
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

void cOper_Add_Plane::EndOperation()
{
	cOperation::EndOperation();
	m_worksSate = cOper_Add_Plane::ADD_PLANE;
	m_position = { 0,0,0 };
	m_rotation = { 0,0,0 };
	m_scale = { 1,1,1 };
}
