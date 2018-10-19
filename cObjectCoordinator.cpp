#include "stdafx.h"

unique_ptr<MeshGeometry> cObjectCoordinator::m_geo = nullptr;
cObjectCoordinator* cObjectCoordinator::instance = nullptr;

cObjectCoordinator::cObjectCoordinator()
	: m_controlObject(nullptr)
	, m_arrowRenderItem(nullptr)
	, m_sphereRenderItem(nullptr)
	, m_arrowRenderInstance(nullptr)
	, m_sphereRenderInstance(nullptr)
	, m_isRotationControl(false)
	, m_prevVec(0, 0, 0)
{
}


void cObjectCoordinator::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{


	m_geo->SetOctree(3);
}

void cObjectCoordinator::SetUp()
{
	m_arrowRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	m_sphereRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	m_arrowRenderItem->SetGeometry(m_geo.get(), "arrow");
	m_sphereRenderItem->SetGeometry(m_geo.get(), "sphere");

	m_arrowRenderInstance = m_arrowRenderItem->GetRenderIsntance();
	m_sphereRenderInstance = m_sphereRenderItem->GetRenderIsntance();
}

void cObjectCoordinator::Update()
{
	if (m_controlObject == nullptr)
	{
		m_arrowRenderInstance->m_isRenderOK = false;
		return;
	}
	else
	{
		m_arrowRenderInstance->m_isRenderOK = true;
	}


	XMVECTOR objCenter = XMLoadFloat3(&m_controlObject->m_pos);
	XMVECTOR objQuter = XMLoadFloat4(&m_controlObject->m_quaternion);
	XMMATRIX objectMat = XMLoadFloat4x4(&m_controlObject->GetMatrix());
	//XMMATRIX rotationMat = XMMatrixRotationQuaternion(objQuter);

	PICKRAY mouseRay = INPUTMG->GetMousePickLay();
	mouseRay.ray = XMVector3TransformNormal(mouseRay.ray, objectMat);
	mouseRay.origin = XMVector3TransformCoord(mouseRay.origin, objectMat);

	float distance;

	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		if (m_geo->octree->Picking(mouseRay, distance))
		{
			XMStoreFloat3(&m_prevVec, (mouseRay.origin + mouseRay.ray*distance) - objCenter);
			m_isRotationControl = true;
		}
	}

	if (!INPUTMG->GetMouseDownStay(VK_LBUTTON))
	{
		m_isRotationControl = false;
	}

	if (m_isRotationControl)
	{
		if (m_geo->octree->Picking(mouseRay, distance))
		{
			XMVECTOR currVec = (mouseRay.origin + mouseRay.ray*distance) - objCenter;

			XMVECTOR rotationValue = XMVector3AngleBetweenVectors(XMLoadFloat3(&m_prevVec), currVec);
			XMVECTOR quterionValue = XMQuaternionRotationRollPitchYawFromVector(rotationValue);

			XMStoreFloat4(&m_controlObject->m_quaternion,XMQuaternionMultiply(objQuter, quterionValue));
			XMStoreFloat3(&m_prevVec, currVec);
		}
	}


}
