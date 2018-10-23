#pragma once

class cOper_Add_Line : public cOperation
{
private:
	enum STATE
	{
		FIRST_DOT_PICK,
		SECEND_DOT_PICK,
	};

public:
	cOper_Add_Line();
	virtual ~cOper_Add_Line();

	virtual void SetUp() override;
	virtual void DrawElementOperation(DrawItems* drawItems) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void EndOperation() override;

private:
	cDot*					m_firstDot;
};

