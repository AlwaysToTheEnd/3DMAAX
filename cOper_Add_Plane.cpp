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

UINT cOper_Add_Plane::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	m_planes = &planes;

	switch (m_worksSate)
	{
	case cOper_Add_Plane::ADD_PLANE:
	{
		m_currPlane = nullptr;
		m_currPlane = planes.AddElement();
		OBJCOORD->ObjectRegistration(m_currPlane);
		m_operControl->AddParameter(L"quater X : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetQuaternion().x);
		m_operControl->AddParameter(L"quater Y : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetQuaternion().y);
		m_operControl->AddParameter(L"quater Z : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetQuaternion().z);
		m_operControl->AddParameter(L"quater W : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetQuaternion().w);
		m_operControl->AddParameter(L"pos    X : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetPos().x);
		m_operControl->AddParameter(L"pos    Y : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetPos().y);
		m_operControl->AddParameter(L"pos    Z : ", OPERDATATYPE_FLOAT, (void*)&m_currPlane->GetPos().z);
		UIMG->UIOn(m_operControl);
		m_worksSate = cOper_Add_Plane::PLANE_ATTRIBUTE_SET;
	}
	break;
	case cOper_Add_Plane::PLANE_ATTRIBUTE_SET:
	{
		if (currMesh)
		{
			if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
			{
				PICKRAY ray = INPUTMG->GetMousePickLay();
				XMMATRIX invObjectMat = XMMatrixInverse(&XMVECTOR(),currMesh->GetXMMatrix());

				PICKRAY objectLocalRay;
				objectLocalRay.origin = XMVector3TransformCoord(ray.origin, invObjectMat);
				objectLocalRay.ray = XMVector3Normalize(XMVector3TransformNormal(ray.ray, invObjectMat));

				float dist = FLT_MAX;
				XMFLOAT4 quaternion;

				if (currMesh->GetPickTriangleInfo(objectLocalRay, XMVectorSet(0, 0, -1, 0), dist, &quaternion))
				{
					m_currPlane->GetQuaternion() = quaternion;
					XMVECTOR pickPos = ray.origin + ray.ray*dist;
					XMStoreFloat3(&m_currPlane->GetPos(), pickPos);
				}
			}
		}
	}
		break;
	default:
		break;
	}

	return 0;
}


void cOper_Add_Plane::CancleOperation(DrawItems* draw)
{
	if (m_planes)
	{
		m_planes->DeleteBackObject();
	}

	m_planes = nullptr;
	EndOperation();
}
