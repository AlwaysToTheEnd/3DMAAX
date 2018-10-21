#include "stdafx.h"

unique_ptr<MeshGeometry> cDrawPlane::m_geo = nullptr;

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

void cDrawPlane::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	if (m_geo == nullptr)
	{
		m_geo = make_unique<MeshGeometry>();
		m_geo->name = "Plane";
		vector<NT_Vertex> vertices(4);

		vertices[0] = { { -0.5f,0.5f,0 },{ 0,0,-1 },{ 0,0 } };
		vertices[1] = { { 0.5f,0.5f,0 },{ 0,0,-1 },{ 1,0 } };
		vertices[2] = { { 0.5f,-0.5f,0 },{ 0,0,-1 },{ 1,1 } };
		vertices[3] = { { -0.5f,-0.5f,0 },{ 0,0,-1 },{ 0,1 } };

		vector<UINT16> Indices;
		Indices.push_back(0);
		Indices.push_back(1);
		Indices.push_back(2);
		Indices.push_back(0);
		Indices.push_back(2);
		Indices.push_back(3);

		const UINT PlaneGeoDataSize = sizeof(NT_Vertex)*vertices.size();
		const UINT PlaneGeoIndexSize = sizeof(UINT16)*Indices.size();

		m_geo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
			vertices.data(), PlaneGeoDataSize, m_geo->vertexUploadBuffer);

		m_geo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
			Indices.data(), PlaneGeoIndexSize, m_geo->indexUploadBuffer);

		m_geo->indexFormat = DXGI_FORMAT_R16_UINT;
		m_geo->indexBufferByteSize = PlaneGeoIndexSize;
		m_geo->vertexBufferByteSize = PlaneGeoDataSize;
		m_geo->vertexByteStride = sizeof(NT_Vertex);

		SubMeshGeometry subMesh;
		subMesh.baseVertexLocation = 0;
		subMesh.startIndexLocation = 0;
		subMesh.indexCount = Indices.size();
		m_geo->DrawArgs["Plane"] = subMesh;
	}
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
	m_renderItem->SetGeometry(m_geo.get(), "Plane");
	m_renderItem->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool cDrawPlane::Picking(cObject ** ppObject)
{
	float dist = FLT_MAX;
	PICKRAY mouseRay = INPUTMG->GetMousePickLay();

	for (auto& it : m_objects)
	{
		float currObjectDist;
		if (it->Picking(mouseRay, currObjectDist))
		{
			if (currObjectDist < dist)
			{
				*ppObject = it.get();
				dist = currObjectDist;
			}
		}
	}

	return dist != FLT_MAX;
}

cObject * cDrawPlane::AddElement()
{
	m_objects.push_back(make_unique<cPlane>());
	m_objects.back()->Build(m_renderItem);
	m_objects.back()->SetSizeScale(1.5f);
	return m_objects.back().get();
}
