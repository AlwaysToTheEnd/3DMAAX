#pragma once

enum OPERATIONTYPE
{
	OPER_NONE,
	OPER_PLANE,
	OPER_LINE,
};

class cOperation
{
public:
	cOperation()
		: m_operType(OPER_NONE)
		, m_operState(false)
	{

	}
	virtual ~cOperation() {}
	virtual void DrawOperation(vector<unique_ptr<cDrawElement>>& draw) {};
	virtual void CancleOperation() = 0;

public:
	void StartOperation() { m_operState = true; }

	bool GetOperState() { return m_operState; }
	OPERATIONTYPE GetOperType() { return m_operType; }

protected:
	const OPERATIONTYPE m_operType;
	bool m_operState;
};

