#pragma once

class cOper_Add_Dot :public cOperation
{
private:
	enum STATE
	{
		CURR_DRAW_CHECK,
		SET_DOTS,
	};

public:
	cOper_Add_Dot();
	virtual ~cOper_Add_Dot();

	virtual UINT OperationUpdate(unordered_map<wstring, DrawItems>& drawItems, 
		cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void EndOperation() override;

private:
	cPlane*		m_currPlane;
	cDot*		m_currDot;
	UINT		m_addDotCount;
	UINT		m_existingDotNum;
};

