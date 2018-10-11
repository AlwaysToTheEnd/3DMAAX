#include "stdafx.h"

string cMirror::ReflectionRenderPipeLineName;

cMirror::cMirror()
	:m_pos(0,0,0)
	,m_rotX(0)
	,m_rotY(0)
{
}


cMirror::~cMirror()
{
}

void cMirror::Update(cObject* renderObj)
{
	auto objRender = renderObj->GetRenderItem();

	m_reflectionRender[0]->geo = objRender->geo;
	m_reflectionRender[0]->mater = objRender->mater;
	m_reflectionRender[0]->texture = objRender->texture;
	m_reflectionRender[0]->numFramesDirty = objRender->numFramesDirty;
	m_reflectionRender[0]->indexCount = objRender->indexCount;
	m_reflectionRender[0]->primitiveType = objRender->primitiveType;
	m_reflectionRender[0]->startIndexLocation = objRender->startIndexLocation;
	m_reflectionRender[0]->baseVertexLocation = objRender->baseVertexLocation;
	m_reflectionRender[0]->isRenderOK = true;

	XMMATRIX R, T;
	T = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	R = XMMatrixRotationY(m_rotY)*XMMatrixRotationX(m_rotX);

	XMStoreFloat4x4(&m_RenderItem->world, R*T);
	XMVECTOR normal = XMVectorSet(0, 0, -1, 0);
	normal = XMVector3TransformNormal(normal, R);

	XMMATRIX reflectionMat = XMMatrixReflect(XMPlaneFromPointNormal(XMLoadFloat3(&m_pos),
		normal));

	XMStoreFloat4x4(&m_reflectionRender[0]->world, XMLoadFloat4x4(&objRender->world)*reflectionMat);
}

void cMirror::Build(string subMeshName, MeshGeometry * geo, Material * mater, 
	D3D12_GPU_DESCRIPTOR_HANDLE tex, string piplineName)
{
	cObject::Build(subMeshName, geo, mater, tex, piplineName);

	for (int i = 0; i < MaxReflectNum; i++)
	{
		m_reflectionRender[i] = RENDERITEMMG->AddRenderItem(ReflectionRenderPipeLineName);
		m_reflectionRender[i]->isRenderOK=false;
	}
}
