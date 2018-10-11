#include "stdafx.h"

unique_ptr<MeshGeometry> cUIObject::m_uiGeo = nullptr;

cUIObject::cUIObject()
	:m_Pos(0,0,0)
{
}


cUIObject::~cUIObject()
{
}

void cUIObject::UIMeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	if (m_uiGeo == nullptr)
	{
		m_uiGeo = make_unique<MeshGeometry>();
		m_uiGeo->name = "UI";
		vector<Vertex> vertices(4);

		vertices[0] = { {0,0,0},{0,0,1},{0,0} };
		vertices[1] = { {1,0,0},{0,0,1},{1,0} };
		vertices[2] = { {1,1,0},{0,0,1},{1,1} };
		vertices[3] = { {0,1,0},{0,0,1},{0,1} };

		vector<UINT16> Indices;
		Indices.push_back(0);
		Indices.push_back(1);
		Indices.push_back(2);
		Indices.push_back(0);
		Indices.push_back(2);
		Indices.push_back(3);

		const UINT uiGeoDataSize = sizeof(Vertex)*vertices.size();
		const UINT uiGeoIndexSize = sizeof(UINT16)*Indices.size();

		m_uiGeo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
			vertices.data(), uiGeoDataSize, m_uiGeo->vertexUploadBuffer);

		m_uiGeo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
			Indices.data(), uiGeoIndexSize, m_uiGeo->indexUploadBuffer);

		m_uiGeo->indexFormat = DXGI_FORMAT_R16_UINT;
		m_uiGeo->indexBufferByteSize = uiGeoIndexSize;
		m_uiGeo->vertexBufferByteSize = uiGeoDataSize;
		m_uiGeo->vertexByteStride = sizeof(Vertex);

		SubMeshGeometry subMesh;
		subMesh.baseVertexLocation = 0;
		subMesh.startIndexLocation = 0;
		subMesh.indexCount = Indices.size();
		m_uiGeo->DrawArgs["UI"] = subMesh;
	}
}

void cUIObject::Update(FXMMATRIX mat)
{
	UIUpdate();
	XMMATRIX translationMat = XMMatrixTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	XMMATRIX scaleMat = XMMatrixScaling(m_Size.x, m_Size.y, 1);
	XMMATRIX uiMat = scaleMat * translationMat*mat;

	XMStoreFloat4x4(&m_RenderItem->world, uiMat);

	m_RenderItem->numFramesDirty = gNumFrameResources;

	for (auto& it : m_ChildObject)
	{
		it->Update(uiMat);
	}
}

void cUIObject::Build(string piplineName)
{
	m_RenderItem = RENDERITEMMG->AddRenderItem(piplineName);
	m_RenderItem->geo = m_uiGeo.get();
	m_RenderItem->indexCount = m_RenderItem->geo->DrawArgs["UI"].indexCount;
	m_RenderItem->baseVertexLocation = m_RenderItem->geo->DrawArgs["UI"].baseVertexLocation;
	m_RenderItem->startIndexLocation = m_RenderItem->geo->DrawArgs["UI"].startIndexLocation;
}

void cUIObject::RenderUI(bool value)
{
	m_RenderItem->isRenderOK = value;

	for (auto& it : m_ChildObject)
	{
		it->m_RenderItem->isRenderOK = value;
	}
}
