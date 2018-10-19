#include "stdafx.h"

unique_ptr<MeshGeometry> cUIObject::m_geo = nullptr;

cUIObject::cUIObject()
{
}


cUIObject::~cUIObject()
{
}

void cUIObject::MeshSetUp(ID3D12Device * device, ID3D12GraphicsCommandList * cmdList)
{
	if (m_geo == nullptr)
	{
		m_geo = make_unique<MeshGeometry>();
		m_geo->name = "UI";
		vector<NT_Vertex> vertices(4);

		vertices[0] = { {0,0,0},{0,0,-1},{0,0} };
		vertices[1] = { {1,0,0},{0,0,-1},{1,0} };
		vertices[2] = { {1,1,0},{0,0,-1},{1,1} };
		vertices[3] = { {0,1,0},{0,0,-1},{0,1} };

		vector<UINT16> Indices;
		Indices.push_back(0);
		Indices.push_back(1);
		Indices.push_back(2);
		Indices.push_back(0);
		Indices.push_back(2);
		Indices.push_back(3);

		const UINT uiGeoDataSize = sizeof(NT_Vertex)*vertices.size();
		const UINT uiGeoIndexSize = sizeof(UINT16)*Indices.size();

		m_geo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
			vertices.data(), uiGeoDataSize, m_geo->vertexUploadBuffer);

		m_geo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(device, cmdList,
			Indices.data(), uiGeoIndexSize, m_geo->indexUploadBuffer);

		m_geo->indexFormat = DXGI_FORMAT_R16_UINT;
		m_geo->indexBufferByteSize = uiGeoIndexSize;
		m_geo->vertexBufferByteSize = uiGeoDataSize;
		m_geo->vertexByteStride = sizeof(NT_Vertex);

		SubMeshGeometry subMesh;
		subMesh.baseVertexLocation = 0;
		subMesh.startIndexLocation = 0;
		subMesh.indexCount = Indices.size();
		m_geo->DrawArgs["UI"] = subMesh;
	}
}

void cUIObject::SetGeoAtRenderItem(shared_ptr<cRenderItem> renderItem)
{
	assert(m_geo.get() && "UIObject Geo didn`t have Setup");

	renderItem->SetGeometry(m_geo.get(), "UI");
}

void XM_CALLCONV cUIObject::Update(FXMMATRIX mat)
{
	UIUpdate();
	XMMATRIX localMat = XMMatrixScaling(m_scale.x, m_scale.y, 1)* XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	XMMATRIX worldMat = localMat * mat;

	XMStoreFloat4x4(&m_renderInstance->instanceData.World, worldMat);
	m_renderInstance->numFramesDirty = gNumFrameResources;

	for (auto& it : m_ChildObject)
	{
		it->Update(worldMat);
	}
}

void cUIObject::SetPos(XMFLOAT3 pos)
{
	m_pos = pos;
}

void cUIObject::SetSize(XMFLOAT2 size)
{
	m_scale.x = size.x;
	m_scale.y = size.y;
}
