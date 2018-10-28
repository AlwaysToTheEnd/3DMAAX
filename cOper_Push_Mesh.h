#pragma once

class cOper_Push_Mesh : public cOperation
{
private:
	enum STATE
	{
		ADD_MESH_QUERY,
		DRAW_SELECT,
		CYCLE_SELECT,
	};

public:
	cOper_Push_Mesh();
	virtual ~cOper_Push_Mesh();

	virtual void SetUp()override;
	virtual void MeshOperation(cMesh* currMesh) override;

	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw);
private:
	DrawItems*	m_draws;
	int			m_selectDrawsIndex;
	list<vector<const cDot*>> m_currDrawCycleDotsList;
};

