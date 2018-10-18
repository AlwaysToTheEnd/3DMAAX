#pragma once
#include "cUIObject.h"
#include "cUIOperWindow.h"

enum OPERATIONTYPE
{
	OPER_ADD_PLANE,
	OPER_ADD_LINE,
	OPER_DRAWOPER_COUNT,
	OPER_PUSH_MESH,
	OPER_MESHOPER_COUNT,
};

class cOperation
{
public:
	static void SetOperatorUIRender(shared_ptr<cRenderItem> renderItem);

public:
	cOperation(OPERATIONTYPE type)
		: m_operType(type)
		, m_operState(false)
	{

	}

	virtual ~cOperation() {}
	virtual void SetUp() = 0;
	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) = 0;
	virtual void CancleOperation() = 0;
	virtual void EndOperation() { m_operState = false; m_operControl.IsRenderState(false); }

public:
	void StartOperation() { m_operState = true; m_operControl.IsRenderState(true); }

	bool GetOperState() const { return m_operState; }
	OPERATIONTYPE GetOperType() const { return m_operType; }

protected:

protected:
	static shared_ptr<cRenderItem> m_OperatorUi;
	const OPERATIONTYPE	m_operType;
	cUIOperWindow		m_operControl;
	bool				m_operState;
};