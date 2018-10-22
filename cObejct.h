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
	void SetSizeScale(float scale) { m_renderInstance->instanceData.sizeScale = scale; }

	XMFLOAT4X4 GetMatrix() const { return m_renderInstance->instanceData.World; }
	XMMATRIX XM_CALLCONV GetXMMatrix() const { return XMLoadFloat4x4(&m_renderInstance->instanceData.World); }
	XMFLOAT3& GetPos() { return m_pos; }
	XMFLOAT4& GetQuaternion() { return m_quaternion; }
	XMFLOAT4 GetQuaternionInstance() const { return m_quaternion; }
	XMFLOAT3& GetScale() { return m_scale; }
	XMFLOAT3 GetWorldPos() const
	{ 
		return { m_renderInstance->instanceData.World._41,
			m_renderInstance->instanceData.World._42, m_renderInstance->instanceData.World._43 };
	}

	friend class cObjectCoordinator;
protected:
	XMFLOAT3					m_pos;
	XMFLOAT4					m_quaternion;
	XMFLOAT3					m_scale;
	shared_ptr<RenderInstance>	m_renderInstance;
};
