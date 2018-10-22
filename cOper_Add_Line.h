#pragma once

class cOper_Add_Line : public cOperation
{
private:
	enum STATE
	{
		SELECT_PLANE,
		FIRST_DOT_PICK,
		SECEND_DOT_PICK,
	};

public:
	cOper_Add_Line();
	virtual ~cOper_Add_Line();

	virtual void SetUp() override;
	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void EndOperation() override;

private:
	cDot * AddDotAtCurrPlane(vector<unique_ptr<cDrawElement>>& draw);

private:
	shared_ptr<RenderFont>	m_operationText;
	cPlane*					m_currPlane;
	cDot*					m_firstDot;
};

