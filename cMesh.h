#pragma once

struct DrawItems;

class cMesh :public cObject
{
public:
	static const char*		m_meshRenderName;

public:
	cMesh();
	virtual ~cMesh();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	virtual void IsRenderState(bool value) { m_renderInstance->m_isRenderOK = value; }

public:
	void SubObjectSubAbsorption();
	void AddCSGObject(CSGWORKTYPE work, unique_ptr<cCSGObject> object);
	bool XM_CALLCONV GetPickTriangleInfo(PICKRAY ray, FXMVECTOR baseDir, float& dist, XMFLOAT4* quaternion) const;
	cCSGObject* GetCSGRootObject() { return m_rootCSG.get(); }

public:
	void SetGeometry(MeshGeometry* geo) { m_geo = geo; }
	UINT SetDrawItems(DrawItems* drawItem);
	vector<DrawItems*>& GetDraws() { return m_draws; }

private:
	MeshGeometry*			m_geo;
	vector<SubMeshGeometry> m_drawArgs;
	BoundingSphere			m_boundSphere;
	vector<DrawItems*>		m_draws;
	shared_ptr<cRenderItem> m_renderItem;

	unique_ptr<cCSGObject>	m_rootCSG;
	vector<NT_Vertex>		m_vertices;
	vector<UINT>			m_indices;
};
