#pragma once

class cOper_Add_Plane : public cOperation
{
private:
	enum STATE
	{
		ADD_PLANE,
		PLANE_ATTRIBUTE_SET,
	};

public:
	cOper_Add_Plane();
	virtual ~cOper_Add_Plane();

	virtual UINT OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
		cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh) override;
	virtual void CancleOperation(DrawItems* draw) override;

private:
	cDrawPlane *	m_planes;
	cObject*		m_currPlane;
};

