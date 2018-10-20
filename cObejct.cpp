#include "stdafx.h"

cObject::cObject()
	: m_renderInstance(nullptr)
	, m_pos(0,0,0)
	, m_quaternion(0,0,0,1)
	, m_scale(1,1,1)
{
}

cObject::~cObject()
{

}

void cObject::Build(shared_ptr<cRenderItem> renderItem)
{
	m_renderInstance = renderItem->GetRenderIsntance();
}

void cObject::Update(FXMMATRIX mat)
{
	XMMATRIX localMat = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * 
		XMMatrixRotationQuaternion(XMLoadFloat4(&m_quaternion))*
		XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	XMMATRIX worldMat = localMat * mat;

	XMStoreFloat4x4(&m_renderInstance->instanceData.World, worldMat);
	m_renderInstance->numFramesDirty = gNumFrameResources;
}
