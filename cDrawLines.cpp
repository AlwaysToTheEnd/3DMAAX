#include "stdafx.h"

unique_ptr<MeshGeometry> cDrawLines::m_geo = nullptr;

cLine::cLine()
	:m_lineDistacne(0)
{
	m_hostObject[0] = nullptr;
	m_hostObject[1] = nullptr;
}

cLine::~cLine()
{
}

void XM_CALLCONV cLine::Update(FXMMATRIX mat)
{
	assert(m_hostObject[0] && m_hostObject[1]);

	XMVECTOR lineVec = XMLoadFloat3(&m_hostObject[1]->GetWorldPos()) - XMLoadFloat3(&m_hostObject[0]->GetWorldPos());
	m_lineDistacne = XMVector3Length(lineVec).m128_f32[0];

	m_pos = m_hostObject[0]->GetWorldPos();
	m_scale = { m_lineDistacne /2 ,1 ,1 };
	XMVECTOR baseVector = XMVectorSet(1, 0, 0, 0);
	XMVECTOR axis = XMVector3Cross(baseVector, lineVec);
	XMStoreFloat4(&m_quaternion, XMQuaternionNormalize(XMQuaternionRotationAxis(axis,
		XMVector3AngleBetweenNormals(XMVector3Normalize(lineVec), baseVector).m128_f32[0])));

	XMMATRIX localMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion))*
		XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	XMMATRIX worldMat = localMat * mat;

	XMStoreFloat4x4(&m_renderInstance->instanceData.World, worldMat);
	m_renderInstance->numFramesDirty = gNumFrameResources;
}

bool XM_CALLCONV cLine::Picking(PICKRAY ray, float & distance)
{
	return false;
}

void cDrawLines::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	vector<C_Vertex> colorVertex;
	vector<UINT> indices;
	auto lineGeo = make_unique<MeshGeometry>();
	lineGeo->name = "line";

	colorVertex.push_back(C_Vertex({ 0,0,0 }, Colors::Black));
	colorVertex.push_back(C_Vertex({ 1,0,0 }, Colors::Black));
	indices.push_back(0);
	indices.push_back(1);

	lineGeo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
		colorVertex.data(), colorVertex.size() * sizeof(C_Vertex), lineGeo->vertexUploadBuffer);

	lineGeo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
		indices.data(), indices.size() * sizeof(UINT), lineGeo->indexUploadBuffer);

	lineGeo->indexFormat = DXGI_FORMAT_R32_UINT;
	lineGeo->indexBufferByteSize = indices.size() * sizeof(UINT);
	lineGeo->vertexBufferByteSize = colorVertex.size() * sizeof(C_Vertex);
	lineGeo->vertexByteStride = sizeof(C_Vertex);

	SubMeshGeometry subMesh;
	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = indices.size();
	lineGeo->DrawArgs["line"] = subMesh;
	m_geo = move(lineGeo);
}

cDrawLines::cDrawLines()
{

}

cDrawLines::~cDrawLines()
{

}

void cDrawLines::SetRenderItem(shared_ptr<cRenderItem> renderItem)
{
	m_renderItem = renderItem;
	m_renderItem->SetGeometry(m_geo.get(), "line");
	m_renderItem->SetPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

bool cDrawLines::Picking(cObject ** ppObject)
{
	return false;
}

cObject * cDrawLines::AddElement()
{
	m_objects.push_back(make_unique<cLine>());
	m_objects.back()->Build(m_renderItem);
	return m_objects.back().get();
}


