#pragma once

class cOper_Add_Line : public cOperation
{
private:
	enum STATE
	{
		SELECT_PLANE,
		FIRST_DOT_PICK,
		SECEND_DOT_PICK,
		OPER_END,
	};

public:
	cOper_Add_Line();
	virtual ~cOper_Add_Line();

	virtual void SetUp() override;
	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;

private:
	shared_ptr<RenderFont> m_operationText;
};

