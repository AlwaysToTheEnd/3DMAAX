#pragma once

class cOper_Add_Draws : public cOperation
{
private:
	enum STATE
	{
		SETUI,
		PICK,
	};

public:
	cOper_Add_Draws();
	virtual ~cOper_Add_Draws();

	virtual void SetUp() override;
	virtual UINT OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
		cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh) override;
	virtual void CancleOperation(DrawItems* draw) override;

private:
	void SetDraws(DrawItems& drawItems);

private:
	vector<shared_ptr<cRenderItem>>	m_renderItems;
	int								m_selectDrawsIndex;
};

