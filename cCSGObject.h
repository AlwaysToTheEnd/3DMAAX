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
	bool TriangleCollision(	const XMFLOAT3& destPos1, const XMFLOAT3& destPos2, const XMFLOAT3& destPos3,
							const XMFLOAT3& srcPos1, const XMFLOAT3& srcPos2, const XMFLOAT3& srcPos3,
							bool collisionLine[]);

private:
	CSGWORKTYPE			m_type;
	const DrawItems*	m_draws;
	vector<NT_Vertex>	m_vertices;
	vector<UINT>		m_indices;
	list<unique_ptr<cCSGObject>> m_childs;
};

