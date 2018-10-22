#pragma once

class cOper_Add_Dot :public cOperation
{
private:
	enum STATE
	{
		SELECT_PLANE,
		SET_DOTS,
	};

public:
	cOper_Add_Dot();
	virtual ~cOper_Add_Dot();

	virtual void SetUp() override;
	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void EndOperation() override;

private:
	cPlane*		m_currPlane;
	cDot*		m_currDot;
	UINT		m_addDotCount;
	UINT		m_existingDotNum;
};

