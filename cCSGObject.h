#pragma once

enum CSGWORKTYPE
{
	CSG_ROOT,
	CSG_UNION,
	CSG_DIFFERENCE,
	CSG_INTERSECTION,
};

class cCSGObject
{
	struct TriangleInfo
	{
		XMFLOAT3	normal = {};
		UINT		index[3] = {};
		bool		ccw = false;
	};

public:
	cCSGObject();
	virtual ~cCSGObject();

	void SubObjectSubAbsorption();
	void AddChild(CSGWORKTYPE type, unique_ptr<cCSGObject> object);
	void GetData(vector<NT_Vertex>& vertices, vector<UINT>& indices);
	void XM_CALLCONV SetData(const vector<NT_Vertex>& vertices, const vector<UINT>& indices, FXMMATRIX mat);

private:

	void ObjectUnion(const cCSGObject* src);
	void ObjectDifference(const cCSGObject* src);
	void ObjectInterSection(const cCSGObject* src);
	bool XM_CALLCONV TriangleCollision(	FXMVECTOR destPos1, FXMVECTOR destPos2, FXMVECTOR destPos3,
										GXMVECTOR srcPos1, HXMVECTOR srcPos2, HXMVECTOR srcPos3,
										bool collisionLine[]);

private:
	CSGWORKTYPE				m_type;
	vector<NT_Vertex>		m_vertices;
	vector<TriangleInfo>	m_triangles;
	list<unique_ptr<cCSGObject>> m_childs;
};

