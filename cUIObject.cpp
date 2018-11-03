#include "stdafx.h"

MeshGeometry* cUIObject::m_geo = nullptr;

cUIObject::cUIObject()
{
}


cUIObject::~cUIObject()
{
}

void cUIObject::MeshSetUp()
{
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

	const UINT uiGeoDataSize = sizeof(NT_Vertex)*(UINT)vertices.size();
	const UINT uiGeoIndexSize = sizeof(UINT16)*(UINT)Indices.size();

	m_geo = MESHMG->AddMeshGeometry("UI", vertices.data(), Indices.data(),
		sizeof(NT_Vertex), uiGeoDataSize, DXGI_FORMAT_R16_UINT, uiGeoIndexSize, false);
}

void cUIObject::SetGeoAtRenderItem(shared_ptr<cRenderItem> renderItem)
{
	assert(m_geo && "UIObject Geo didn`t have Setup");

	renderItem->SetGeometry(m_geo, "UI");
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
