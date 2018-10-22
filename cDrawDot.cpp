#include "stdafx.h"

unique_ptr<MeshGeometry> cDrawDot::m_geo = nullptr;

cDot::cDot()
	:m_hostObject(nullptr)
{
}

cDot::~cDot()
{

}

void XM_CALLCONV cDot::Update(FXMMATRIX mat)
{
	assert(m_hostObject);

	XMMATRIX localMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion))*
		XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	XMMATRIX worldMat = localMat * mat * XMLoadFloat4x4(&m_hostObject->GetMatrix());

	XMStoreFloat4x4(&m_renderInstance->instanceData.World, worldMat);
	m_renderInstance->numFramesDirty = gNumFrameResources;
}

bool XM_CALLCONV cDot::Picking(PICKRAY ray, float & distance)
{
	return false;
}

void cDrawDot::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	GeometryGenerator gen;
	vector<C_Vertex> vertices;
	m_geo = make_unique<MeshGeometry>();
	m_geo->name = "dot";

	GeometryGenerator::MeshData sphere = gen.CreateSphere(0.01f, 6, 6);

	vertices.resize(sphere.Vertices.size());

	for (int i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i].pos = sphere.Vertices[i].Position;
		XMStoreFloat4(&vertices[i].color, Colors::Gray.v);
	}

	const UINT vertexSize = vertices.size() * sizeof(C_Vertex);
	const UINT indicesSize = sphere.Indices32.size() * sizeof(UINT16);

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

	m_geo->DrawArgs["dot"] = subMesh;
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
	m_renderItem->SetGeometry(m_geo.get(), "dot");
	m_renderItem->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool cDrawDot::Picking(cObject ** ppObject)
{
	return false;
}

cObject * cDrawDot::AddElement()
{
	m_objects.push_back(make_unique<cDot>());
	m_objects.back()->Build(m_renderItem);
	return m_objects.back().get();
}

