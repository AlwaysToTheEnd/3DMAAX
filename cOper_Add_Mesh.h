#pragma once

#include "cDrawMesh.h"

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

	virtual void SetUp() override;
	virtual DrawItems* MeshSelectOperation(unordered_map<wstring, DrawItems>& drawItems, cDrawMesh& drawMesh, cMesh** mesh) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;

private:
	int m_selectDrawsIndex;
};

