#pragma once

class cOper_Add_Mesh : public cOperation
{
	enum STATE
	{
		SETUI,
		PICK,
	};

public:
	cOper_Add_Mesh();
	virtual ~cOper_Add_Mesh();

	virtual void SetUp() override;
	virtual DrawItems* DrawSelectOperation(unordered_map<wstring, DrawItems>& drawItems) override;
	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) override;

private:
	int m_selectDrawsIndex;
};

