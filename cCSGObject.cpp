#include "stdafx.h"


cCSGObject::cCSGObject()
	: m_type(CSG_ROOT)
	, m_isOn(true)
{
}


cCSGObject::~cCSGObject()
{
}

void cCSGObject::SubObjectSubAbsorption()
{
	if (m_type == CSG_ROOT)
	{
		m_vertices.clear();
		m_triangles.clear();
	}

	for (auto& it : m_childs)
	{
		if (it->m_isOn)
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
}

//void cCSGObject::ChildOnstateControl(cCSGObject * childObjectAddress, bool value)
//{
//	for (auto& it : m_childs)
//	{
//		if (childObjectAddress == it.get())
//		{
//			if (childObjectAddress->m_isOn != value)
//			{
//				childObjectAddress->m_isOn = value;
//				break;
//			}
//		}
//	}
//}

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
		XMVECTOR destPos0 = XMLoadFloat3(&m_vertices[m_triangles[i].index[0]].pos);
		XMVECTOR destPos1 = XMLoadFloat3(&m_vertices[m_triangles[i].index[1]].pos);
		XMVECTOR destPos2 = XMLoadFloat3(&m_vertices[m_triangles[i].index[2]].pos);
		XMVECTOR collsionPos1 = XMVectorZero();
		XMVECTOR collsionPos2 = XMVectorZero();
		UINT index0 = UINT_MAX;
		UINT index1 = UINT_MAX;

		for (UINT j = 0; j < srcTriangleSize; j++)
		{
			XMVECTOR srcPos0 = XMLoadFloat3(&srcVertices[srcTriangles[i].index[0]].pos);
			XMVECTOR srcPos1 = XMLoadFloat3(&srcVertices[srcTriangles[i].index[1]].pos);
			XMVECTOR srcPos2 = XMLoadFloat3(&srcVertices[srcTriangles[i].index[2]].pos);

			switch (TriangleCollision(	destPos0, destPos1, destPos2, 
										srcPos0, srcPos1, srcPos2, 
										collsionPos1, collsionPos2,
										index0, index1))
			{
			case cCSGObject::EACH_OTHER:
				break;
			case cCSGObject::DEST_2POINT:
				break;
			case cCSGObject::SRC_2POINT:
				break;
			default:
				break;
			}
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
		XMStoreFloat(&dot, XMVector3Dot(crossVector, normal));

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

UINT XM_CALLCONV cCSGObject::TriangleCollision(	FXMVECTOR destPos0, FXMVECTOR destPos1, FXMVECTOR destPos2,
												GXMVECTOR srcPos0, HXMVECTOR srcPos1, HXMVECTOR srcPos2,
												XMVECTOR& collisionPos1, XMVECTOR& collisionPos2,
												UINT& index0, UINT& index1)
{
	XMVECTOR		destOrigin[3] = {};
	XMVECTOR		srcOrigin[3] = {};
	XMVECTOR		destRay[3] = {};
	XMVECTOR		srcRay[3] = {};
	vector<UINT>	destRayCollsionIndices;
	vector<UINT>	srcRayCollsionIndices;
	XMVECTOR		destCollisionPos[3] = {};
	XMVECTOR		srcCollisionPos[3] = {};

	destOrigin[0] = destPos0;
	destOrigin[1] = destPos1;
	destOrigin[2] = destPos2;

	srcOrigin[0] = srcPos0;
	srcOrigin[1] = srcPos1;
	srcOrigin[2] = srcPos2;

	destRay[0] = destOrigin[1] - destOrigin[0];
	destRay[1] = destOrigin[2] - destOrigin[0];
	destRay[2] = destOrigin[2] - destOrigin[1];

	srcRay[0] = srcOrigin[1] - srcOrigin[0];
	srcRay[1] = srcOrigin[2] - srcOrigin[0];
	srcRay[2] = srcOrigin[2] - srcOrigin[1];

	float dist = FLT_MAX;
	for (UINT i = 0; i < 3; i++)
	{
		if (TriangleTests::Intersects(destOrigin[i], destRay[i], srcOrigin[0], srcOrigin[1], srcOrigin[2], dist))
		{
			if (dist > 0.0f && dist < 1.0f)
			{
				destCollisionPos[i] = destOrigin[i] + destRay[i] * dist;
				destRayCollsionIndices.push_back(i);
			}
		}
	}

	for (UINT i = 0; i < 3; i++)
	{
		if (TriangleTests::Intersects(srcOrigin[i], srcRay[i], destOrigin[0], destOrigin[1], destOrigin[2], dist))
		{
			if (dist > 0.0f && dist < 1.0f)
			{
				srcCollisionPos[i] = srcOrigin[i] + srcRay[i] * dist;
				srcRayCollsionIndices.push_back(i);
			}
		}
	}

	if (destRayCollsionIndices.size() == 1 && srcRayCollsionIndices.size() == 1)
	{
		collisionPos1 = destCollisionPos[destRayCollsionIndices[0]];
		collisionPos2 = srcCollisionPos[srcRayCollsionIndices[0]];
		index0 = destRayCollsionIndices[0];
		index1 = srcRayCollsionIndices[0];
		return EACH_OTHER;
	}
	else if (destRayCollsionIndices.size() == 2)
	{
		collisionPos1 = destCollisionPos[destRayCollsionIndices[0]];
		collisionPos2 = destCollisionPos[destRayCollsionIndices[1]];
		index0 = destRayCollsionIndices[0];
		index1 = destRayCollsionIndices[1];
		return DEST_2POINT;
	}
	else if (srcRayCollsionIndices.size() == 2)
	{
		collisionPos1 = srcCollisionPos[srcRayCollsionIndices[0]];
		collisionPos2 = srcCollisionPos[srcRayCollsionIndices[1]];
		index0 = srcRayCollsionIndices[0];
		index1 = srcRayCollsionIndices[1];
		return SRC_2POINT;
	}

	return COLLISION_NONE;
}

