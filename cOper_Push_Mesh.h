#pragma once

class cOper_Push_Mesh : public cOperation
{
private:
	enum STATE
	{
		ADD_MESH_QUERY,
		VERTEX_CHECK,
		DRAW_SELECT,
	};

public:
	cOper_Push_Mesh();
	virtual ~cOper_Push_Mesh();

	virtual void SetUp()override;
	virtual void MeshOperation(cDrawMesh& drawMesh, DrawItems* drawItem, cMesh** currMesh);
	virtual DrawItems* MeshSelectOperation(unordered_map<wstring, DrawItems>& drawItems, 
		cDrawMesh& drawMesh, cMesh** mesh);

	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw);
	virtual void EndOperation();
private:
	cMesh *		m_currMesh;
	DrawItems*	m_draws;
	int			m_selectDrawsIndex;
};

