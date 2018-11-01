#include "stdafx.h"


cOper_Add_Plane::cOper_Add_Plane()
	: cOperation(OPER_ADD_PLANE)
	, m_planes(nullptr)
	, m_currPlane(nullptr)
{

}


cOper_Add_Plane::~cOper_Add_Plane()
{

}

void cOper_Add_Plane::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.SetPos({ 650,100,0 });
}

void cOper_Add_Plane::PlaneAddOperation(cDrawPlane & planes, cMesh* currMesh)
{
	m_planes = &planes;

	switch (m_worksSate)
	{
	case cOper_Add_Plane::ADD_PLANE:
	{
		m_currPlane = nullptr;
		m_currPlane = planes.AddElement();
		OBJCOORD->ObjectRegistration(m_currPlane);
		m_operControl.AddParameter(L"quater X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetQuaternion().x);
		m_operControl.AddParameter(L"quater Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetQuaternion().y);
		m_operControl.AddParameter(L"quater Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetQuaternion().z);
		m_operControl.AddParameter(L"quater W : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetQuaternion().w);
		m_operControl.AddParameter(L"pos    X : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetPos().x);
		m_operControl.AddParameter(L"pos    Y : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetPos().y);
		m_operControl.AddParameter(L"pos    Z : ", DXGI_FORMAT_R32_FLOAT, (void*)&m_currPlane->GetPos().z);
		m_operControl.SetRenderState(true);
		m_worksSate = cOper_Add_Plane::PLANE_ATTRIBUTE_SET;
	}
	break;
	case cOper_Add_Plane::PLANE_ATTRIBUTE_SET:
	{
		if (currMesh)
		{
			if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
			{
				float dist = FLT_MAX;
				PICKRAY ray = INPUTMG->GetMousePickLay();
				currMesh->GetPickTriangleRotation(ray, &m_currPlane->GetQuaternion());
			}
		}
	}
		m_operControl.Update(XMMatrixIdentity());
		break;
	default:
		break;
	}
}


void cOper_Add_Plane::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	if (m_planes)
	{
		m_planes->DeleteBackObject();
	}

	m_planes = nullptr;
	EndOperation();
}
