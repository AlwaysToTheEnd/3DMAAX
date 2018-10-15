#pragma once
class cOperator
{
public:
	cOperator();
	~cOperator();

	void SetUp();

private:
	cOperation* m_currOperation;
	vector<unique_ptr<cOperation>> m_operations;

	cButtonCollector m_drawButtonCollector;
	cButtonCollector m_meshButtonCollector;
};

