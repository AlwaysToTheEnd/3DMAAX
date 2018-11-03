#pragma once

struct DrawItems;

class cMesh :public cObject
{
public:
	static const char*		m_meshRenderName;

private:
	struct CycleLine
	{
		cLine* line = nullptr;
		CycleLine* parentLine = nullptr;

		vector<CycleLine> nextLines;

		CycleLine() {}
		CycleLine(cLine* line, CycleLine* parentLine)
		{
			this->line = line;
			this->parentLine = parentLine;
		}

		void CycleLineCheck(list<cLine*>& leaveLines,
			cLine * endLinkLine, int endLinkPoint, list<vector<const cDot*>>& cycleDots);

		vector<const cDot*> GetDotsToParents();
	};

public:
	cMesh();
	virtual ~cMesh();

	virtual void Build(shared_ptr<cRenderItem> renderItem) override;
	virtual void XM_CALLCONV Update(FXMMATRIX mat) override;
	virtual bool XM_CALLCONV Picking(PICKRAY ray, float& distance) override;
	virtual void IsRenderState(bool value) { m_renderInstance->m_isRenderOK = value; }
	
public:
	list<vector<const cDot*>> LineCycleCheck(UINT drawItemIndex);
	void SubObjectSubAbsorption();
	void AddCSGObject(CSGWORKTYPE work, unique_ptr<cCSGObject> object);
	bool XM_CALLCONV GetPickTriangleInfo(PICKRAY ray, FXMVECTOR baseDir, float& dist, XMFLOAT4* quaternion);

public:
	void SetGeometry(MeshGeometry* geo) { m_geo = geo; }
	UINT SetDrawItems(DrawItems* drawItem);
	vector<DrawItems*>& GetDraws() { return m_draws; }

private:
	void OverLapCycleDotsCheck(list<vector<const cDot*>>& dotslist);
	bool EqualCheck(vector<const cDot*>& lhs, vector<const cDot*>& rhs);

private:
	MeshGeometry*			m_geo;
	shared_ptr<cRenderItem> m_renderItem;
	BoundingSphere			m_boundSphere;
	vector<DrawItems*>		m_draws;

	vector<SubMeshGeometry> m_drawArgs;
	cCSGObject				m_rootCSG;
	vector<NT_Vertex>		m_vertices;
	vector<UINT>			m_indices;
};
