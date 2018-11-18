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
private:
	struct TriangleInfo
	{
		XMFLOAT3	normal = {};
		UINT		index[3] = {};
		bool		ccw = false;
	};

	enum TRIANGLECOLLISIONTYPE
	{
		COLLISION_NONE,
		EACH_OTHER,
		DEST_2POINT,
		SRC_2POINT,
	};

public:
	cCSGObject();
	virtual ~cCSGObject();

	void SubObjectSubAbsorption();
	void AddChild(CSGWORKTYPE type, unique_ptr<cCSGObject> object);
	void GetData(vector<NT_Vertex>& vertices, vector<UINT>& indices) const;
	void XM_CALLCONV SetData(const vector<NT_Vertex>& vertices, const vector<UINT>& indices, FXMMATRIX mat);
	void SetOnState(bool value) { m_isOn = value; }

	vector<unique_ptr<cCSGObject>>& GetChilds() { return m_childs; }
	bool GetOnState() const { return m_isOn; }
	CSGWORKTYPE GetType() const { return m_type; }

private:
	void ObjectUnion(const cCSGObject* src);
	void ObjectDifference(const cCSGObject* src);
	void ObjectInterSection(const cCSGObject* src);
	UINT XM_CALLCONV TriangleCollision(	FXMVECTOR destPos0, FXMVECTOR destPos1, FXMVECTOR destPos2,
										GXMVECTOR srcPos0, HXMVECTOR srcPos1, HXMVECTOR srcPos2,
										XMFLOAT3& collisionPos0, XMFLOAT3& collisionPos1,
										UINT& index0, UINT& index1);

	UINT GetLeaveIndex(UINT index0, UINT index1) const;
private:
	bool					m_isOn;
	CSGWORKTYPE				m_type;
	vector<NT_Vertex>		m_vertices;
	vector<TriangleInfo>	m_triangles;
	vector<unique_ptr<cCSGObject>> m_childs;
};

