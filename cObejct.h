#pragma once

class cObject
{
public:
	cObject();
	virtual ~cObject();

	virtual void Build(shared_ptr<cRenderItem> renderItem);
	virtual void XM_CALLCONV Update(FXMMATRIX mat);
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) { assert(false); return false; }
	virtual void IsRenderState(bool value) { m_renderInstance->m_isRenderOK = value; }

	void SetMatrialIndex(UINT index) { m_renderInstance->instanceData.MaterialIndex = index; }
	const XMFLOAT4X4& GetMatrix() { return m_renderInstance->instanceData.World; }
	const XMFLOAT3& GetPos() { return m_pos; }
	const XMFLOAT4& GetQuaternion() { return m_quaternion; }
	const XMFLOAT3& GetScale() { return m_scale; }

	friend class cObjectCoordinator;
protected:
	XMFLOAT3					m_pos;
	XMFLOAT4					m_quaternion;
	XMFLOAT3					m_scale;
	shared_ptr<RenderInstance>	m_renderInstance;
};
