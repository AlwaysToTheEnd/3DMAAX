#include "stdafx.h"

MeshGeometry* cDrawDot::m_geo = nullptr;

cDot::cDot()
	:m_hostObject(nullptr)
{
}

cDot::~cDot()
{

}

void XM_CALLCONV cDot::Update(FXMMATRIX mat)
{
	XMMATRIX localMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion))*
		XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	XMMATRIX worldMat = localMat * mat * XMLoadFloat4x4(&m_hostObject->GetMatrix());

	XMStoreFloat4x4(&m_renderInstance->instanceData.World, worldMat);
	m_renderInstance->numFramesDirty = gNumFrameResources;
}

bool XM_CALLCONV cDot::Picking(PICKRAY ray, float & distance)
{
	BoundingSphere sphere;
	sphere.Center = GetWorldPos();
	sphere.Radius = 0.02f;

	return sphere.Intersects(ray.origin,ray.ray,distance);
}

void cDot::SetNextDots(cDot * dot)
{
	for (auto& it : m_nextDots)
	{
		if (dot == it) return;
	}

	m_nextDots.push_back(dot);
}

void cDrawDot::MeshSetUp()
{
	GeometryGenerator gen;
	vector<C_Vertex> vertices;

	GeometryGenerator::MeshData sphere = gen.CreateSphere(0.01f, 6, 6);

	vertices.resize(sphere.Vertices.size());

	for (int i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i].pos = sphere.Vertices[i].Position;
		XMStoreFloat4(&vertices[i].color, Colors::Black.v);
	}

	const UINT vertexSize = (UINT)vertices.size() * sizeof(C_Vertex);
	const UINT indicesSize = (UINT)sphere.Indices32.size() * sizeof(UINT16);

	m_geo = MESHMG->AddMeshGeometry("dot", vertices.data(), sphere.GetIndices16().data(),
		sizeof(C_Vertex), vertexSize, DXGI_FORMAT_R16_UINT, indicesSize, false);
}

cDrawDot::cDrawDot()
{

}


cDrawDot::~cDrawDot()
{

}

void cDrawDot::SetRenderItem(shared_ptr<cRenderItem> renderItem)
{
	m_renderItem = renderItem;
	m_renderItem->SetGeometry(m_geo, "dot");
	m_renderItem->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

cObject * cDrawDot::AddElement()
{
	m_objects.push_back(make_unique<cDot>());
	m_objects.back()->Build(m_renderItem);
	m_objects.back()->SetPickRender(m_pickRender);
	return m_objects.back().get();
}

