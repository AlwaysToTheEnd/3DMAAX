#pragma once

#include "cMesh.h"

class cOper_Add_Mesh : public cOperation
{
	enum STATE
	{
		SETUI,
		PICK,
	};

public:
	cOper_Add_Mesh();
	virtual ~cOper_Add_Mesh();

	virtual UINT OperationUpdate(	unordered_map<wstring, DrawItems>& drawItems,
									cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs,
									DrawItems*& currDrawItems, cMesh*& currMesh) override;

	virtual void CancleOperation(DrawItems* draw) override;

private:
	int m_selectDrawsIndex;
};

