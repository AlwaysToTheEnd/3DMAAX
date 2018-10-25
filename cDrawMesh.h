#pragma once

struct DrawItems;

class cMesh :public cObject
{
private:

public:
	cMesh();
	virtual ~cMesh();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	virtual void IsRenderState(bool value) { m_renderInstance->m_isRenderOK = value; }
	
public:
	bool VertexCircleCheck(UINT drawItemIndex);

public:
	void SetGeometry(MeshGeometry* geo) { m_geo = geo; }
	UINT SetDrawItems(DrawItems* drawItem);
	vector<DrawItems*>& GetDraws() { return m_draws; }

private:
	MeshGeometry*			m_geo;
	shared_ptr<cRenderItem> m_renderItem;
	BoundingSphere			m_boundSphere;
	vector<DrawItems*>		m_draws;

	vector<SubMeshGeometry> m_drawArgs;
	vector<NT_Vertex>		m_vertices;
	vector<UINT>			m_indices;
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

