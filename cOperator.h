#pragma once
#define NOMALBUTTONSIZE 30


class cOperator
{
public:
	cOperator();
	~cOperator();

	void SetUp();
	void Update(vector<unique_ptr<cDrawElement>>& draws, cDrawPlane* drawPlane);

	void SetRenderState(bool value);

private:
	void OperationStart(int type);
	void OperTypeSelect(int num);
	void SetObjectRenderItems();

private:
	const int						m_ButtonMtlTexBaseIndex = 0;
	shared_ptr<cRenderItem>			m_buttonRenderItem;
	cButtonCollector				m_operSelectButtons;
	cButtonCollector				m_drawButtons;
	cButtonCollector				m_meshButtons;
	cButtonCollector*				m_currButtons;

	vector<shared_ptr<cRenderItem>>	m_renderItems;
	vector<unique_ptr<cOperation>>	m_operations;

	vector<unique_ptr<cDrawElement>>* m_currDraws;
	cOperation*						m_currOperation;
};

