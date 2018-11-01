#include "stdafx.h"


cCSGObject::cCSGObject()
	:m_type(CSG_ROOT)
{
}


cCSGObject::~cCSGObject()
{
}

void cCSGObject::SubObjectSubAbsorption()
{
	for (auto& it : m_childs)
	{
		it->SubObjectSubAbsorption();

		switch (m_type)
		{
		case CSG_UNION:
			ObjectUnion(it.get());
			break;
		case CSG_DIFFERENCE:
			ObjectUnion(it.get());
			break;
		case CSG_INTERSECTION:
			ObjectUnion(it.get());
			break;
		default:
			assert(false);
			break;
		}
	}
}

void cCSGObject::AddChild(CSGWORKTYPE type, unique_ptr<cCSGObject> object)
{
	object->m_type = type;
	m_childs.push_back(move(object));
}

void cCSGObject::ObjectUnion(const cCSGObject * src)
{
	auto srcVertices = src->m_vertices;
	auto srcTriangles = src->m_triangles;

	UINT srcVerticesSize = (UINT)srcVertices.size();
	UINT srcTriangleSize = (UINT)srcTriangles.size();

	if (m_vertices.empty())
	{
		m_vertices.assign(srcVertices.cbegin(), srcVertices.cend());
		m_triangles.assign(srcTriangles.cbegin(), srcTriangles.cend());
		return;
	}

	for (UINT i = 0; i < srcVerticesSize; i++)
	{
		//for()
	}
}

void cCSGObject::ObjectDifference(const cCSGObject * src)
{
	if (m_vertices.empty()) return;

	auto srcVertices = src->m_vertices;
	auto srcTriangles = src->m_triangles;

	for (auto& it : srcTriangles)
	{
		it.ccw = !it.ccw;
	}

	UINT srcVerticesSize = (UINT)srcVertices.size();
	UINT srcTriangleSize = (UINT)srcTriangles.size();

	m_vertices.insert(m_vertices.end(), srcVertices.cbegin(), srcVertices.cend());

	list<UINT> modifiedIndex;

	for (UINT i = 0; i < srcTriangleSize; i++)
	{
		for (UINT j = 0; j < srcTriangleSize; j++)
		{
			
		}
	}
}

void cCSGObject::ObjectInterSection(const cCSGObject * src)
{
	if (m_vertices.empty()) return;

	auto srcVertices = src->m_vertices;
	auto srcTriangles = src->m_triangles;

	UINT srcVerticesSize = (UINT)srcVertices.size();
	UINT srcTriangleSize = (UINT)srcTriangles.size();
}

bool XM_CALLCONV cCSGObject::TriangleCollision( FXMVECTOR destPos1, FXMVECTOR destPos2, FXMVECTOR destPos3, 
												GXMVECTOR srcPos1, HXMVECTOR srcPos2, HXMVECTOR srcPos3, 
												bool collisionLine[])
{

	XMVECTOR destRay1 = destPos2 - destPos1;
	XMVECTOR destRay2 = destPos3 - destPos1;
	XMVECTOR destRay3 = destPos3 - destPos2;

	XMVECTOR srcRay1 = srcPos2 - srcPos1;
	XMVECTOR srcRay2 = srcPos3 - srcPos1;
	XMVECTOR srcRay3 = srcPos3 - srcPos2;




	return false;
}

