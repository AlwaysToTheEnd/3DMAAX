#pragma once

class cMesh :public cObject
{
public:
	cMesh();
	virtual ~cMesh();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	virtual void IsRenderState(bool value) { m_renderInstance->m_isRenderOK = value; }
	
public:
	void SetGeometry(MeshGeometry* geo) { m_geo = geo; }

private:
	MeshGeometry*			m_geo;
	shared_ptr<cRenderItem> m_renderItem;
	BoundingSphere			m_boundSphere;
	vector<DrawItems>		m_draws;
};

class cDrawMesh : public cDrawElement
{
public:
	static const string m_meshRenderName;

public:
	cDrawMesh();
	virtual ~cDrawMesh();

	virtual void SetRenderItem(shared_ptr<cRenderItem> renderItem) {}
	virtual cObject* AddElement() override;

private:
};

