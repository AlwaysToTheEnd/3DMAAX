#pragma once

class cOper_Add_Plane : public cOperation
{
private:
	enum STATE
	{
		ADD_PLANE,
		PLANE_ATTRIBUTE_SET,
		OPER_END,
	}m_worksSate;

public:
	cOper_Add_Plane();
	virtual ~cOper_Add_Plane();

	virtual void SetUp() override;
	virtual void DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw) override;
	virtual void CancleOperation() override { m_operState = false; m_operControl.IsRenderState(false); }
	virtual void EndOperation() override;

private:
	XMFLOAT3	m_position;
	XMFLOAT3	m_rotation;
	XMFLOAT3	m_scale;
};

