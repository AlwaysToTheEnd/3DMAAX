#pragma once

class cObject
{
public:
	cObject();
	virtual ~cObject();

	virtual void Build(string piplineName = "");
	virtual void Build(string subMeshName, MeshGeometry* geo, Material* mater, D3D12_GPU_DESCRIPTOR_HANDLE tex, string piplineName="");
	virtual void Update(FXMMATRIX mat);
	virtual void Update() {}

	shared_ptr<RenderItem> GetRenderItem() { return m_RenderItem; }

protected:
	shared_ptr<RenderItem>			m_RenderItem;
};
