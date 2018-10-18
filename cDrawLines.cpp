#include "stdafx.h"

unique_ptr<MeshGeometry> cDrawLines::m_geo = nullptr;

void cDrawLines::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	vector<C_Vertex> colorVertex;
	vector<UINT> indices;
	auto lineGeo = make_unique<MeshGeometry>();
	lineGeo->name = "line";

	colorVertex.push_back(C_Vertex({ 0,0,0 }, Colors::White));
	colorVertex.push_back(C_Vertex({ 1,0,0 }, Colors::White));
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

void cDrawLines::Update()
{

}

void cDrawLines::SetRenderItem(shared_ptr<cRenderItem> renderItem)
{

}
