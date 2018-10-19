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
	GeometryGenerator gen;
	vector<C_Vertex> vertices;
	m_geo = make_unique<MeshGeometry>();
	m_geo->name = "objectCoordinator";

	GeometryGenerator::MeshData sphere = gen.CreateSphere(0.5f, 10, 10);

	vertices.resize(sphere.Vertices.size());

	for (int i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i].pos = sphere.Vertices[i].Position;
		XMStoreFloat4(&vertices[i].color, Colors::Gray.v);
	}

	const UINT vertexSize = vertices.size() * sizeof(C_Vertex);
	const UINT indicesSize = sphere.Indices32.size() * sizeof(UINT16);

	D3DCreateBlob(vertexSize, m_geo->vertexBufferCPU.GetAddressOf());
	memcpy(m_geo->vertexBufferCPU->GetBufferPointer(), vertices.data(), vertexSize);

	D3DCreateBlob(indicesSize, m_geo->indexBufferCPU.GetAddressOf());
	memcpy(m_geo->indexBufferCPU->GetBufferPointer(), sphere.GetIndices16().data(), indicesSize);

	m_geo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
		vertices.data(), vertexSize, m_geo->vertexUploadBuffer);

	m_geo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
		sphere.GetIndices16().data(), indicesSize, m_geo->indexUploadBuffer);

	m_geo->indexFormat = DXGI_FORMAT_R16_UINT;
	m_geo->indexBufferByteSize = indicesSize;
	m_geo->vertexBufferByteSize = vertexSize;
	m_geo->vertexByteStride = sizeof(C_Vertex);

	SubMeshGeometry subMesh;
	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = sphere.Indices32.size();

	m_geo->DrawArgs["sphere"] = subMesh;
	m_geo->SetOctree(0);
}

void cObjectCoordinator::SetUp()
{
	//m_arrowRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	//m_arrowRenderItem->SetGeometry(m_geo.get(), "arrow");

	m_sphereRenderItem = RENDERITEMMG->AddRenderItem("objectCoordinator");
	m_sphereRenderItem->SetGeometry(m_geo.get(), "sphere");
	m_sphereRenderInstance = m_sphereRenderItem->GetRenderIsntance();
	m_arrowRenderInstance = m_sphereRenderItem->GetRenderIsntance();

	m_arrowRenderInstance->instanceData.World._11 = 0.2f;
	m_arrowRenderInstance->instanceData.World._22 = 0.2f;
	m_arrowRenderInstance->instanceData.World._33 = 0.2f;
}

void cObjectCoordinator::Update()
{
	if (m_controlObject == nullptr)
	{
		m_sphereRenderInstance->m_isRenderOK = false;
		return;
	}
	else
	{
		m_sphereRenderInstance->instanceData.World = m_controlObject->GetMatrix();
		m_sphereRenderInstance->numFramesDirty = gNumFrameResources;
		m_sphereRenderInstance->m_isRenderOK = true;
		m_arrowRenderInstance->m_isRenderOK = true;
		m_arrowRenderInstance->numFramesDirty = gNumFrameResources;
	}

	XMVECTOR objCenter = XMLoadFloat3(&m_controlObject->m_pos);
	XMVECTOR objRotation = XMLoadFloat3(&m_controlObject->m_rotation);
	XMMATRIX objectMat = XMLoadFloat4x4(&m_controlObject->GetMatrix());
	XMMATRIX invObjMat = XMMatrixInverse(&XMVECTOR(), objectMat);
	//XMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(objRotation);

	PICKRAY mouseRay = INPUTMG->GetMousePickLay();
	mouseRay.ray = XMVector3Normalize(XMVector3TransformNormal(mouseRay.ray, invObjMat));
	mouseRay.origin = XMVector3TransformCoord(mouseRay.origin, invObjMat);

	float distance;

	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		if (m_geo->octree->Picking(mouseRay, distance))
		{
			XMVECTOR pickPos = (mouseRay.origin + mouseRay.ray*distance);
			m_arrowRenderInstance->instanceData.World._41 = pickPos.m128_f32[0];
			m_arrowRenderInstance->instanceData.World._42 = pickPos.m128_f32[1];
			m_arrowRenderInstance->instanceData.World._43 = pickPos.m128_f32[2];
			XMStoreFloat3(&m_prevVec, pickPos - objCenter);
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
			XMVECTOR rotationValue = XMVector3AngleBetweenVectors(XMLoadFloat3(&m_prevVec),currVec);

			XMStoreFloat3(&m_controlObject->m_rotation, objRotation + rotationValue);
			XMStoreFloat3(&m_prevVec, currVec);
		}
		else
		{
			m_isRotationControl = false;
		}
	}
}
