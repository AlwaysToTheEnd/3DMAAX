#include "stdafx.h"

cObject::cObject()
	: m_RenderItem(nullptr)
{
}

cObject::~cObject()
{

}

void cObject::Build(string piplineName)
{
	m_RenderItem = RENDERITEMMG->AddRenderItem(piplineName);
}

void cObject::Build(string subMeshName, MeshGeometry * geo, Material * mater,
	D3D12_GPU_DESCRIPTOR_HANDLE tex, string piplineName)
{
	m_RenderItem = RENDERITEMMG->AddRenderItem(piplineName);

	m_RenderItem->geo = geo;
	m_RenderItem->mater = mater;
	m_RenderItem->texture = tex;
	m_RenderItem->indexCount = geo->DrawArgs[subMeshName].indexCount;
	m_RenderItem->baseVertexLocation = geo->DrawArgs[subMeshName].baseVertexLocation;
	m_RenderItem->startIndexLocation = geo->DrawArgs[subMeshName].startIndexLocation;
}

void cObject::Update(FXMMATRIX mat)
{

}