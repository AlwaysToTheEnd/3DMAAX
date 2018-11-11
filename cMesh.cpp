#include "stdafx.h"

const char* cMesh::m_meshRenderName = "baseMesh";

cMesh::cMesh()
	: m_geo(nullptr)
	, m_renderItem(nullptr)
	, m_rootCSG(nullptr)
{

}

cMesh::~cMesh()
{
}

void cMesh::Build(shared_ptr<cRenderItem> renderItem)
{
	static UINT meshNum = 0;

	m_rootCSG = make_unique<cCSGObject>();
	m_renderItem = RENDERITEMMG->AddRenderItem(m_meshRenderName);
	m_geo = MESHMG->AddTemporaryMesh(m_meshRenderName + to_string(meshNum));

	m_renderItem->SetGeometry(m_geo, m_meshRenderName + to_string(meshNum));
	m_renderItem->SetRenderOK(false);

	m_renderInstance = m_renderItem->GetRenderIsntance();
	m_renderInstance->instanceData.MaterialIndex = 7;

	meshNum++;
}

void XM_CALLCONV cMesh::Update(FXMMATRIX mat)
{
	cObject::Update(mat);
}

bool XM_CALLCONV cMesh::Picking(PICKRAY ray, float & distance)
{
	return m_geo->octree->Picking(ray, distance);
}

void cMesh::SubObjectSubAbsorption()
{
	m_rootCSG->SubObjectSubAbsorption();
	m_rootCSG->GetData(m_vertices, m_indices);

	if (m_vertices.size())
	{
		MESHMG->ChangeMeshGeometryData(m_geo->name, m_vertices.data(), m_indices.data(), sizeof(NT_Vertex),
			sizeof(NT_Vertex)*(UINT)m_vertices.size(), DXGI_FORMAT_R32_UINT, sizeof(UINT)*(UINT)m_indices.size(),
			true);

		m_geo->octree = nullptr;
		m_geo->SetOctree(0);
		m_renderItem->SetGeometry(m_geo, m_geo->name);
		m_renderItem->SetRenderOK(true);
	}
	else
	{
		m_renderItem->SetRenderOK(false);
	}
}

void cMesh::AddCSGObject(CSGWORKTYPE work, unique_ptr<cCSGObject> object)
{
	m_rootCSG->AddChild(work, move(object));
}

bool XM_CALLCONV cMesh::GetPickTriangleInfo(PICKRAY ray, FXMVECTOR baseDir, float& dist, XMFLOAT4 * quaternion) const
{
	if (m_geo->octree == nullptr) return false;

	XMFLOAT3 pos0, pos1, pos2;

	if (m_geo->octree->GetPickTriangle(ray, dist, pos0, pos1, pos2))
	{
		XMVECTOR _pos0 = XMLoadFloat3(&pos0);
		XMVECTOR _pos1 = XMLoadFloat3(&pos1);
		XMVECTOR _pos2 = XMLoadFloat3(&pos2);

		if (quaternion)
		{
			XMVECTOR v1 = _pos1 - _pos0;
			XMVECTOR v2 = _pos2 - _pos0;
			XMVECTOR triangleNormal = XMVector3Cross(v1, v2);
			XMVECTOR frontDir = baseDir;
			XMVECTOR quaternionAxis = XMVector3Cross(frontDir, triangleNormal);

			float angle = 0;
			XMStoreFloat(&angle, XMVector3AngleBetweenVectors(frontDir, triangleNormal));

			if (XMVector3Equal(quaternionAxis, XMVectorZero()))
			{
				XMMATRIX rotationMat = XMMatrixRotationX(XM_PIDIV2);
				quaternionAxis = XMVector3TransformNormal(baseDir, rotationMat);
			}

			XMVECTOR TriangleQuaternion = XMQuaternionRotationAxis(quaternionAxis, angle);
			XMStoreFloat4(quaternion, TriangleQuaternion);
		}

		return true;
	}

	return false;
}


UINT cMesh::SetDrawItems(DrawItems * drawItem)
{
	UINT indexNum = 0;
	for (auto& it : m_draws)
	{
		if (drawItem == it) return indexNum;
		indexNum++;
	}

	m_draws.push_back(drawItem);
	return indexNum;
}
