#pragma once

enum OPERATIONTYPE
{
	OPER_ADD_PLANE,
	OPER_ADD_LINE,
};

class cOperation
{
public:
	cOperation(OPERATIONTYPE type)
		: m_operType(type)
		, m_operState(false)
	{

	}
	virtual ~cOperation() {}
	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) {};
	virtual void CancleOperation() = 0;

public:
	void StartOperation() { m_operState = true; }

	bool GetOperState() const { return m_operState; }
	OPERATIONTYPE GetOperType() const { return m_operType; }

protected:
	const OPERATIONTYPE m_operType;
	bool m_operState;
};