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
	const auto& srcVertices = src->m_vertices;
	const auto& srcIndices = src->m_indices;

	UINT srcVerticesSize = (UINT)srcVertices.size();
	UINT srcIndicesSize = (UINT)srcIndices.size();

	if (m_vertices.empty())
	{
		m_vertices.assign(srcVertices.cbegin(), srcVertices.cend());
		m_indices.assign(srcIndices.cbegin(), srcIndices.cend());
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

	const auto& srcVertices = src->m_vertices;
	const auto& srcIndices = src->m_indices;

	UINT srcVerticesSize = (UINT)srcVertices.size();
	UINT srcIndicesSize = (UINT)srcIndices.size();

	m_vertices.insert(m_vertices.end(), srcVertices.cbegin(), srcVertices.cend());

	list<UINT> modifiedIndex;

	for (UINT i = 0; i < srcIndicesSize; i+=3)
	{
		UINT tryIndex1 = srcIndices[i];
		UINT tryIndex2 = srcIndices[i+1];
		UINT tryIndex3 = srcIndices[i+2];

		for (UINT j = 0; j < srcIndicesSize; j+=3)
		{
			UINT srcTryIndex1 = srcIndices[j];
			UINT srcTryIndex2 = srcIndices[j + 1];
			UINT srcTryIndex3 = srcIndices[j + 2];

			
		}
	}
}

void cCSGObject::ObjectInterSection(const cCSGObject * src)
{
	if (m_vertices.empty()) return;

	const auto& srcVertices = src->m_vertices;
	const auto& srcIndices = src->m_indices;

	UINT srcVerticesSize = (UINT)srcVertices.size();
	UINT srcIndicesSize = (UINT)srcIndices.size();
}

bool cCSGObject::TryangleCollision(const XMFLOAT3 & destPos1, const XMFLOAT3 & destPos2, const XMFLOAT3 & destPos3, const XMFLOAT3 & srcPos1, const XMFLOAT3 & srcPos2, const XMFLOAT3 & srcPos3, int collisionLine[])
{

	return false;
}


