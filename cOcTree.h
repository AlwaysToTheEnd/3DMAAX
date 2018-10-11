#pragma once

#define OCTREE_TREENUM 8
//TEST

class cOcTree
{
private:
	class OctreeNode
	{
		friend class cOcTree;

	public:
		OctreeNode(XMFLOAT3 Left_Top_Front_pos, XMFLOAT3 size, const MeshGeometry* geo);
		OctreeNode(XMVECTOR Left_Top_Front_pos, XMVECTOR size, const MeshGeometry* geo);
		~OctreeNode() {}
		
	private:
		void CreateChildNode();
		bool InterSect(int tryangleNum);
		void AddTriangle(UINT num,int maxRecursion);

		bool XM_CALLCONV Picking(PICKRAY ray, float& dist);

	private:
		array<unique_ptr<OctreeNode>, OCTREE_TREENUM> m_nodes = {};
		const MeshGeometry* m_geo;
		unique_ptr<vector<int>> m_triAngles;
		BoundingBox boundBox;
	};

public:
	virtual ~cOcTree();

	static cOcTree* CreateOcTree(const MeshGeometry* geo, int maxRecursion);
	bool XM_CALLCONV Picking(PICKRAY ray, float& dist);

private:
	cOcTree();

	const MeshGeometry* m_geo;
	unique_ptr<OctreeNode> m_Root;
	UINT m_maxRecursion;
};




