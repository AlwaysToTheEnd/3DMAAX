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
		bool		ccw = true;
	};

public:
	cCSGObject();
	virtual ~cCSGObject();

private:
	void SubObjectSubAbsorption();
	void AddChild(CSGWORKTYPE type, unique_ptr<cCSGObject> object);

	void ObjectUnion(const cCSGObject* src);
	void ObjectDifference(const cCSGObject* src);
	void ObjectInterSection(const cCSGObject* src);

private:
	bool XM_CALLCONV TriangleCollision(	FXMVECTOR destPos1, FXMVECTOR destPos2, FXMVECTOR destPos3,
										GXMVECTOR srcPos1, HXMVECTOR srcPos2, HXMVECTOR srcPos3,
										bool collisionLine[]);

private:
	CSGWORKTYPE				m_type;
	const DrawItems*		m_draws;
	vector<NT_Vertex>		m_vertices;
	vector<TriangleInfo>	m_triangles;
	list<unique_ptr<cCSGObject>> m_childs;
};

