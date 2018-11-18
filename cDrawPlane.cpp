#include "stdafx.h"

MeshGeometry* cDrawPlane::m_geo = nullptr;

cPlane::cPlane()
{
}

cPlane::~cPlane()
{

}

bool XM_CALLCONV cPlane::Picking(PICKRAY ray, float & distance)
{
	XMMATRIX world = XMLoadFloat4x4(&m_renderInstance->instanceData.World);
	XMMATRIX invWorld = XMMatrixInverse(&XMVECTOR(), world);

	PICKRAY planeLocalRay;
	planeLocalRay.ray = XMVector3TransformNormal(ray.ray, invWorld);
	planeLocalRay.origin = XMVector3TransformCoord(ray.origin, invWorld);

	XMVECTOR plane = XMPlaneFromPointNormal(XMVectorSet(0, 0, 0, 1), XMVectorSet(0, 0, 1, 0));
	float normalDot = XMVector3Dot(plane, planeLocalRay.ray).m128_f32[0];

	if (normalDot <= 0) return false;

	distance = (XMVector3Dot(-plane, planeLocalRay.origin).m128_f32[0] - plane.m128_f32[3]) / normalDot;
	XMVECTOR pos = planeLocalRay.origin + planeLocalRay.ray*distance;

	const float sizeScale = m_renderInstance->instanceData.sizeScale;
	if (pos.m128_f32[0] > -0.5f*sizeScale &&pos.m128_f32[0] < 0.5f*sizeScale)
	{
		if (pos.m128_f32[1] > -0.5f*sizeScale &&pos.m128_f32[1] < 0.5f*sizeScale)
		{
			return true;
		}
	}

	return false;
}

void cDrawPlane::MeshSetUp()
{
	vector<C_Vertex> vertices(4);

	vertices[0] = { { -0.5f,0.5f,0 },Colors::Yellow };
	vertices[1] = { { 0.5f,0.5f,0 },Colors::Yellow };
	vertices[2] = { { 0.5f,-0.5f,0 },Colors::Yellow };
	vertices[3] = { { -0.5f,-0.5f,0 }, Colors::Yellow };

	for (int i = 0; i < 4; i++)
	{
		vertices[i].color.w = 0.5f;
	}

	vector<UINT16> Indices;
	Indices.push_back(0);
	Indices.push_back(1);
	Indices.push_back(2);
	Indices.push_back(0);
	Indices.push_back(2);
	Indices.push_back(3);

	const UINT PlaneGeoDataSize = sizeof(C_Vertex)*(UINT)vertices.size();
	const UINT PlaneGeoIndexSize = sizeof(UINT16)*(UINT)Indices.size();

	m_geo = MESHMG->AddMeshGeometry("Plane", vertices.data(), Indices.data(),
		sizeof(C_Vertex), PlaneGeoDataSize, DXGI_FORMAT_R16_UINT, PlaneGeoIndexSize, false);
}


cDrawPlane::cDrawPlane()
{

}

cDrawPlane::~cDrawPlane()
{

}

void cDrawPlane::SetRenderItem(shared_ptr<cRenderItem> renderItem)
{
	m_renderItem = renderItem;
	m_renderItem->SetGeometry(m_geo, "Plane");
	m_renderItem->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

cObject * cDrawPlane::AddElement()
{
	m_objects.push_back(make_unique<cPlane>());
	m_objects.back()->Build(m_renderItem);
	m_objects.back()->SetSizeScale(1.5f);
	return m_objects.back().get();
}
