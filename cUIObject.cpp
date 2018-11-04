#include "stdafx.h"

cUIObject::cUIObject()
{
}


cUIObject::~cUIObject()
{
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

void cUIObject::SetRenderState(bool value)
{
	cObject::SetRenderState(value);
}
