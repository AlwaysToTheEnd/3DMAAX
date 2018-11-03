#include "stdafx.h"


cCSGObject::cCSGObject()
	: m_type(CSG_ROOT)
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

		switch (it->m_type)
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
	else
	{
		UINT addVertexStartIndex = (UINT)m_vertices.size();
		UINT addTriangleStartIndex = (UINT)m_triangles.size();
		m_triangles.reserve(m_triangles.size() + srcTriangles.size());
		m_vertices.insert(m_vertices.end(), srcVertices.begin(), srcVertices.end());
		m_triangles.insert(m_triangles.end(), srcTriangles.begin(), srcTriangles.end());

		UINT i = 0;
		for (auto& it : srcTriangles)
		{
			UINT currIndex = i + addTriangleStartIndex;
			m_triangles[currIndex].index[0] = addVertexStartIndex + it.index[0];
			m_triangles[currIndex].index[1] = addVertexStartIndex + it.index[1];
			m_triangles[currIndex].index[2] = addVertexStartIndex + it.index[2];

			i++;
		}
	}

	//for (UINT i = 0; i < srcVerticesSize; i++)
	//{
	//	//for()
	//}
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

void cCSGObject::GetData(vector<NT_Vertex>& vertices, vector<UINT>& indices)
{
	vertices.clear();
	indices.clear();
	vertices.assign(m_vertices.begin(), m_vertices.end());
	
	for (auto& it : m_triangles)
	{
		indices.push_back(it.index[0]);

		if (it.ccw)
		{
			indices.push_back(it.index[2]);
			indices.push_back(it.index[1]);
		}
		else
		{
			indices.push_back(it.index[1]);
			indices.push_back(it.index[2]);
		}
	}
}

void XM_CALLCONV cCSGObject::SetData(const vector<NT_Vertex>& vertices, const vector<UINT>& indices, FXMMATRIX mat)
{
	assert(indices.size() % 3 == 0);
	m_vertices.clear();
	m_triangles.clear();

	if (m_vertices.capacity() < vertices.size())
	{
		m_vertices.reserve(vertices.size());
	}

	XMMATRIX transMatrix = mat;

	for (size_t i = 0; i < vertices.size(); i++)
	{
		NT_Vertex vertex;
		vertex.uv = vertices[i].uv;
		XMStoreFloat3(&vertex.pos, XMVector3TransformCoord(XMLoadFloat3(&vertices[i].pos), transMatrix));
		XMStoreFloat3(&vertex.normal, XMVector3TransformNormal(XMLoadFloat3(&vertices[i].normal), transMatrix));
		m_vertices.push_back(vertex);
	}

	TriangleInfo triangle;
	for (size_t i = 0; i < indices.size(); i += 3)
	{
		triangle.index[0] = indices[i];
		triangle.index[1] = indices[i + 1];
		triangle.index[2] = indices[i + 2];

		triangle.normal = vertices[indices[i]].normal;

		XMVECTOR originVector = XMLoadFloat3(&vertices[indices[i]].pos);
		XMVECTOR tryVector1 = XMLoadFloat3(&vertices[indices[i + 1]].pos) - originVector;
		XMVECTOR tryVector2 = XMLoadFloat3(&vertices[indices[i + 2]].pos) - originVector;

		XMVECTOR crossVector = XMVector3Normalize(XMVector3Cross(tryVector1, tryVector2));
		XMVECTOR normal = XMLoadFloat3(&triangle.normal);

		float dot = 0; 
		XMStoreFloat(&dot,XMVector3Dot(crossVector, normal));

		if (dot < 0)
		{
			triangle.ccw = true;
		}
		else
		{
			triangle.ccw = false;
		}

		m_triangles.push_back(triangle);
	}
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

