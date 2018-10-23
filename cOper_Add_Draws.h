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
	virtual DrawItems* DrawsAddOperatioin(unordered_map<wstring, DrawItems>& drawItems, cDrawPlane& planes) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;

private:
	void SetDraws(DrawItems& drawItems);

private:
	vector<shared_ptr<cRenderItem>>	m_renderItems;
	int								m_selectDrawsIndex;
};

