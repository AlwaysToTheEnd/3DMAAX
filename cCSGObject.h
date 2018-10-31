#pragma once

enum CSGWORKTYPE
{
	CSG_ROOT,
	CSG_UINON,
	CSG_DIFFERENCE,
	CSG_INTERSECTION,
};

class cCSGObject
{
public:
	cCSGObject();
	virtual ~cCSGObject();

private:
	XMFLOAT4X4			m_mat;
	CSGWORKTYPE			type;
	const DrawItems*	m_draws;
	vector<NT_Vertex>	m_vertices;
	vector<UINT>		m_indices;
	vector<unique_ptr<cCSGObject>> m_childs;
};

