#pragma once
#define NOMALBUTTONSIZE 10

class cOperator
{
public:
	cOperator();
	~cOperator();

	void SetUp();
	void Update(vector<unique_ptr<cDrawElement>>& planes);

private:
	void OperationStart(int type);

private:
	static const int m_ButtonMtlTexBaseIndex = 0;

	cOperation* m_currOperation;
	vector<unique_ptr<cOperation>> m_operations;

	shared_ptr<cRenderItem> m_buttonRenderItem;
	cButtonCollector m_drawButtonCollector;
	cButtonCollector m_meshButtonCollector;
};

