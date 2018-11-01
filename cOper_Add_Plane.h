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

	virtual void SetUp() override;
	virtual void PlaneAddOperation(cDrawPlane& planes, cMesh* currMesh) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;

private:
	cDrawPlane *	m_planes;
	cObject*		m_currPlane;
};

