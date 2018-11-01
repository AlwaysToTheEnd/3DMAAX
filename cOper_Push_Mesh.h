#pragma once

class cOper_Push_Mesh : public cOperation
{
private:
	enum STATE
	{
		ADD_MESH_QUERY,
		DRAW_SELECT,
		CYCLE_SELECT,
		MESH_HEIGHT_INPUT
	};

public:
	cOper_Push_Mesh();
	virtual ~cOper_Push_Mesh();

	virtual void SetUp()override;
	virtual void MeshOperation(cMesh* currMesh) override;

	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw);

private:
	void PreviewPushMeshCreate(cMesh* currMesh);
	XMVECTOR XM_CALLCONV GetNormalFromTriangle(const XMFLOAT3& pos1, const XMFLOAT3& pos2, const XMFLOAT3& pos3);

private:
	DrawItems*	m_draws;
	int			m_selectDrawsIndex;
	int			m_cycleIndex;
	float		m_meshHeight;
	bool		m_isCreateMesh;

	vector<NT_Vertex>			m_vertices;
	vector<UINT>				m_indices;
	list<vector<const cDot*>>	m_currDrawCycleDotsList;
};

