#include "stdafx.h"
#include "cOcTree.h"


cOcTree::OctreeNode::OctreeNode(XMFLOAT3 Left_Top_Front, XMFLOAT3 size, const MeshGeometry* geo)
{
	boundBox.Center = { Left_Top_Front.x + size.x / 2,
						Left_Top_Front.y - size.y / 2,
						Left_Top_Front.z + size.z / 2 };

	boundBox.Extents = { size.x / 2,size.y / 2 ,size.z / 2 };

	m_triAngles = make_unique<vector<int>>();
	m_geo = geo;
}

cOcTree::OctreeNode::OctreeNode(XMVECTOR Left_Top_Front, XMVECTOR size, const MeshGeometry* geo)
{
	boundBox.Center = { Left_Top_Front.m128_f32[0] + size.m128_f32[0] / 2,
						Left_Top_Front.m128_f32[1] - size.m128_f32[1] / 2,
						Left_Top_Front.m128_f32[2] + size.m128_f32[2] / 2 };

	XMStoreFloat3(&boundBox.Extents, size / 2);

	m_triAngles = make_unique<vector<int>>();
	m_geo = geo;
}


void cOcTree::OctreeNode::AddTriangle(UINT num, int maxRecursion)
{
	if (maxRecursion < 0) return;

	if (m_triAngles == nullptr)
	{
		for (int i = 0; i < OCTREE_TREENUM; i++)
		{
			if (m_nodes[i]->InterSect(num))
			{
				m_nodes[i]->AddTriangle(num, maxRecursion-1);
				break;
			}
		}
	}
	else
	{
		if (maxRecursion > 0 && m_triAngles->size() > 0)
		{
			CreateChildNode();

			for (int i = 0; i < OCTREE_TREENUM; i++)
			{
				if (m_nodes[i]->InterSect(num))
				{
					m_nodes[i]->AddTriangle(num, maxRecursion-1);
					break;
				}
			}
		}
		else
		{
			m_triAngles->push_back(num);
		}
	}
}

bool XM_CALLCONV cOcTree::OctreeNode::Picking(PICKRAY ray, float & dist)
{
	bool isPicked = false;

	if (m_triAngles == nullptr)
	{
		for (int i = 0; i < OCTREE_TREENUM; i++)
		{
			float distance = FLT_MAX;
			if (m_nodes[i]->boundBox.Intersects(ray.origin, ray.ray, distance))
			{
				if (distance < dist)
				{
					if (m_nodes[i]->Picking(ray, dist))
					{
						isPicked = true;
					}
				}
			}
		}
	}
	else
	{
		BYTE* vertexs = reinterpret_cast<BYTE*>(m_geo->vertexBufferCPU->GetBufferPointer());
		BYTE* indices = reinterpret_cast<BYTE*>(m_geo->indexBufferCPU->GetBufferPointer());
		UINT vertexByteStride = m_geo->vertexByteStride;

		for (auto& it : *m_triAngles)
		{
			XMFLOAT3* vertexPosition0 = nullptr;
			XMFLOAT3* vertexPosition1 = nullptr;
			XMFLOAT3* vertexPosition2 = nullptr;

			switch (m_geo->indexFormat)
			{
			case DXGI_FORMAT_R16_UINT:
			{
				UINT16* currIndex = reinterpret_cast<UINT16*>(indices + it * 3 * sizeof(UINT16));
				vertexPosition0 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition1 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition2 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
			}
			break;
			case DXGI_FORMAT_R32_UINT:
			{
				UINT* currIndex = reinterpret_cast<UINT*>(indices + it * 3 * sizeof(UINT));
				vertexPosition0 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition1 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition2 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
			}
			break;
			default:
				assert(false);
				break;
			}
			
			float distance = FLT_MAX;
			if (TriangleTests::Intersects(ray.origin, ray.ray, XMLoadFloat3(vertexPosition0),
				XMLoadFloat3(vertexPosition1), XMLoadFloat3(vertexPosition2), distance))
			{
				isPicked = true;

				if (dist > distance)
				{
					dist = distance;
				}
			}
		}
	}

	return isPicked;
}

bool XM_CALLCONV cOcTree::OctreeNode::GetPickTriangle(PICKRAY ray, float & dist, XMFLOAT3 & triPos0, XMFLOAT3 & triPos1, XMFLOAT3 & triPos2)
{
	bool isPicked = false;

	if (m_triAngles == nullptr)
	{
		for (int i = 0; i < OCTREE_TREENUM; i++)
		{
			float distance = FLT_MAX;
			if (m_nodes[i]->boundBox.Intersects(ray.origin, ray.ray, distance))
			{
				if (distance < dist)
				{
					if (m_nodes[i]->Picking(ray, dist))
					{
						isPicked = true;
					}
				}
			}
		}
	}
	else
	{
		BYTE* vertexs = reinterpret_cast<BYTE*>(m_geo->vertexBufferCPU->GetBufferPointer());
		BYTE* indices = reinterpret_cast<BYTE*>(m_geo->indexBufferCPU->GetBufferPointer());
		UINT vertexByteStride = m_geo->vertexByteStride;

		for (auto& it : *m_triAngles)
		{
			XMFLOAT3* vertexPosition0 = nullptr;
			XMFLOAT3* vertexPosition1 = nullptr;
			XMFLOAT3* vertexPosition2 = nullptr;

			switch (m_geo->indexFormat)
			{
			case DXGI_FORMAT_R16_UINT:
			{
				UINT16* currIndex = reinterpret_cast<UINT16*>(indices + it * 3 * sizeof(UINT16));
				vertexPosition0 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition1 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition2 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
			}
			break;
			case DXGI_FORMAT_R32_UINT:
			{
				UINT* currIndex = reinterpret_cast<UINT*>(indices + it * 3 * sizeof(UINT));
				vertexPosition0 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition1 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
				currIndex++;
				vertexPosition2 = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
			}
			break;
			default:
				assert(false);
				break;
			}

			float distance = FLT_MAX;
			if (TriangleTests::Intersects(ray.origin, ray.ray, XMLoadFloat3(vertexPosition0),
				XMLoadFloat3(vertexPosition1), XMLoadFloat3(vertexPosition2), distance))
			{
				isPicked = true;

				if (dist > distance)
				{
					triPos0 = *vertexPosition0;
					triPos1 = *vertexPosition1;
					triPos2 = *vertexPosition2;
					dist = distance;

				}
			}
		}
	}

	return isPicked;
}

void cOcTree::OctreeNode::CreateChildNode()
{
	assert(m_nodes[0] == nullptr);

	XMFLOAT3 childNodePos = boundBox.Center;
	XMFLOAT3 childSize = boundBox.Extents;
	childNodePos = childNodePos - childSize;

	childNodePos.y += childSize.y * 2; //this position is LTF on current octreeNode`s Boundbox;
	m_nodes[0] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);
	childNodePos.x += childSize.x; // กๆ
	m_nodes[1] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);
	childNodePos.y -= childSize.y; // ก้
	m_nodes[2] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);
	childNodePos.x -= childSize.x; // ก็
	m_nodes[3] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);

	childNodePos.y += childSize.y; // ก่ 
	childNodePos.z += childSize.z; // ขึ  this position is LTB on current octreeNode`s Boundbox;
	m_nodes[4] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);
	childNodePos.x += childSize.x; // กๆ
	m_nodes[5] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);
	childNodePos.y -= childSize.y; // ก้
	m_nodes[6] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);
	childNodePos.x -= childSize.x; // ก็
	m_nodes[7] = make_unique<cOcTree::OctreeNode>(childNodePos, childSize, m_geo);

	for (int i = 0; i < OCTREE_TREENUM; i++)
	{
		for (int j = 0; j < m_triAngles->size(); j++)
		{
			int currTriangleNum = (*m_triAngles)[j];

			if (m_nodes[i]->InterSect(currTriangleNum))
			{
				m_nodes[i]->m_triAngles->push_back(currTriangleNum);
				(*m_triAngles)[j] = -1;
				break;
			}
		}
	}

	m_triAngles = nullptr;
}

bool cOcTree::OctreeNode::InterSect(int triangleNum)
{
	if (triangleNum < 0) return false;

	BYTE* vertexs = reinterpret_cast<BYTE*>(m_geo->vertexBufferCPU->GetBufferPointer());
	BYTE* indices = reinterpret_cast<BYTE*>(m_geo->indexBufferCPU->GetBufferPointer());
	UINT vertexByteStride = m_geo->vertexByteStride;

	XMFLOAT3 RTBPos = boundBox.Center + boundBox.Extents;
	XMFLOAT3 LBFPos = boundBox.Center - boundBox.Extents;

	XMFLOAT3* vertexPosition=nullptr;

	for (int i = 0; i < 3; i++)
	{
		switch (m_geo->indexFormat)
		{
		case DXGI_FORMAT_R16_UINT:
		{
			UINT16* currIndex = reinterpret_cast<UINT16*>(indices + triangleNum * 3 * sizeof(UINT16));
			currIndex += i;
			vertexPosition = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
		}
		break;
		case DXGI_FORMAT_R32_UINT:
		{
			UINT* currIndex = reinterpret_cast<UINT*>(indices + triangleNum * 3 * sizeof(UINT));
			currIndex += i;
			vertexPosition = reinterpret_cast<XMFLOAT3*>(vertexs + vertexByteStride * (*currIndex));
		}
		break;
		default:
			assert(false);
			break;
		}

		if (vertexPosition->x >= LBFPos.x&&vertexPosition->x <= RTBPos.x)
		{
			if (vertexPosition->y >= LBFPos.y&&vertexPosition->y <= RTBPos.y)
			{
				if (vertexPosition->z >= LBFPos.z&&vertexPosition->z <= RTBPos.z)
				{
					return true;
				}
			}
		}
	}

	return false;
}


cOcTree::cOcTree()
	: m_geo(nullptr)
	, m_Root(nullptr)
	, m_maxRecursion(0)
{

}


cOcTree::~cOcTree()
{

}

cOcTree * cOcTree::CreateOcTree(const MeshGeometry * geo, int numRecursion)
{
	cOcTree* tree = new cOcTree;
	tree->m_geo = geo;
	tree->m_maxRecursion = numRecursion;

	BYTE* vertexs = reinterpret_cast<BYTE*>(geo->vertexBufferCPU->GetBufferPointer());
	BYTE* indices = reinterpret_cast<BYTE*>(geo->indexBufferCPU->GetBufferPointer());

	UINT vertexByteStride = geo->vertexByteStride;
	UINT indexByteSize = geo->indexFormat == DXGI_FORMAT_R16_UINT ? sizeof(UINT16) : sizeof(UINT);
	UINT vertexCount = (UINT)geo->vertexBufferCPU->GetBufferSize() / vertexByteStride;
	UINT triangleCount = (UINT)geo->indexBufferCPU->GetBufferSize() / indexByteSize / 3;

	XMFLOAT3 maxPos = { -FLT_MAX,-FLT_MAX,-FLT_MAX };
	XMFLOAT3 minPos = { FLT_MAX,FLT_MAX ,FLT_MAX };
	XMFLOAT3* vertexPos = nullptr;

	for (UINT i = 0; i < vertexCount; i++)
	{
		vertexPos = reinterpret_cast<XMFLOAT3*>(vertexs + i * vertexByteStride);
		maxPos.x = MathHelper::Max(vertexPos->x, maxPos.x);
		maxPos.y = MathHelper::Max(vertexPos->y, maxPos.y);
		maxPos.z = MathHelper::Max(vertexPos->z, maxPos.z);

		minPos.x = MathHelper::Min(vertexPos->x, minPos.x);
		minPos.y = MathHelper::Min(vertexPos->y, minPos.y);
		minPos.z = MathHelper::Min(vertexPos->z, minPos.z);
	}

	XMFLOAT3 boundBoxSize = { maxPos.x - minPos.x, maxPos.y - minPos.y, maxPos.z - minPos.z };
	XMFLOAT3 LTFPos = { minPos.x, maxPos.y, minPos.z };
	tree->m_Root = make_unique<OctreeNode>(LTFPos, boundBoxSize, geo);

	for (UINT i = 0; i < triangleCount; i++)
	{
		tree->m_Root->AddTriangle(i, numRecursion);
	}

	return tree;
}

bool cOcTree::Picking(PICKRAY ray, float & dist)
{
	dist = FLT_MAX;
	return m_Root->Picking(ray, dist);
}

bool XM_CALLCONV cOcTree::GetPickTriangle(PICKRAY ray, float & dist, XMFLOAT3 & triPos0, XMFLOAT3 & triPos1, XMFLOAT3 & triPos2)
{
	dist = FLT_MAX;
	return m_Root->GetPickTriangle(ray, dist, triPos0, triPos1, triPos2);
}

