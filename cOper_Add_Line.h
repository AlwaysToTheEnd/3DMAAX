#pragma once

class cOper_Add_Line : public cOperation
{
private:
	enum STATE
	{
		CURR_DRAW_CHECK,
		FIRST_DOT_PICK,
		SECEND_DOT_PICK,
	};

public:
	cOper_Add_Line();
	virtual ~cOper_Add_Line();

	virtual UINT OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
		cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh) override;
	virtual void CancleOperation(DrawItems* draw) override;
	virtual void EndOperation() override;

private:
	cDot*					m_firstDot;
	UINT					m_addLineNum = 0;
	UINT					m_addDotNum = 0;
};

