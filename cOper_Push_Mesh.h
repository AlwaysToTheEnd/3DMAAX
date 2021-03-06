#pragma once

class cOper_Push_Mesh : public cOperation
{
private:
	enum STATE
	{
		ADD_MESH_QUERY,
		DRAW_SELECT,
		CYCLE_SELECT,
		MESH_ATTRIBUTE_INPUT
	};

public:
	cOper_Push_Mesh();
	virtual ~cOper_Push_Mesh();

	virtual UINT OperationUpdate(	unordered_map<wstring, DrawItems>& drawItems,
									cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs,
									DrawItems*& currDrawItems, cMesh*& currMesh) override;

	virtual void CancleOperation(DrawItems* draw) override;

private:
	void PreviewPushMeshCreate(cMesh* currMesh);

private:
	DrawItems*	m_draws;
	int			m_cycleIndex;
	float		m_meshHeight;
	bool		m_isCreateMesh;

	CSGWORKTYPE					m_workType;
	vector<NT_Vertex>			m_vertices;
	vector<UINT>				m_indices;
	list<vector<const cDot*>>	m_currDrawCycleDotsList;
};

