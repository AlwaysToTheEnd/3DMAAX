#include "stdafx.h"

unique_ptr<MeshGeometry> cDrawPlane::m_geo = nullptr;

cPlane::cPlane()
	: m_plane(0,0,-1,0)
	, m_viewSize(1,1)
	, m_lines(this)
{
}

cPlane::cPlane(XMFLOAT4 plane)
	: m_plane(plane)
	, m_viewSize(1, 1)
	, m_lines(this)
{

}

cPlane::~cPlane()
{

}

void cPlane::Update()
{
	if (GetAsyncKeyState('1'))
	{
		m_viewSize.x += 2*DELTATIME;
		m_viewSize.y += 2*DELTATIME;
	}

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR normal = XMLoadFloat4(&m_plane);
	XMVECTOR origin;
	normal.m128_f32[3] = 0;
	float distance = m_plane.z;
	origin = normal * distance;

	XMMATRIX scaleMat = XMMatrixScaling(m_viewSize.x, m_viewSize.y, 1);
	XMStoreFloat4x4(&m_renderInstance->instanceData.World, scaleMat* XMMatrixLookAtLH(origin, normal, up));
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

void cDrawPlane::Update()
{

	for (auto& it : m_planes)
	{
		it.Update();
	}
}

void cDrawPlane::SetRenderItem(shared_ptr<cRenderItem> renderItem)
{
	m_renderItem = renderItem;
	m_renderItem->SetGeometry(m_geo.get(), "Plane");
	m_renderItem->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void cDrawPlane::AddPlanes()
{
	m_planes.push_back({});
	m_planes.back().Build(m_renderItem);
}

void cDrawPlane::DeletePlane()
{
	if (m_planes.size())
	{
		m_planes.pop_back();
	}
}

