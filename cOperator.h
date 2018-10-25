#pragma once
#define NOMALBUTTONSIZE 30

class cOperator
{
public:
	cOperator();
	~cOperator();

	void SetUp();
	void Update();

	void SetRenderState(bool value);

private:
	void OperationStart(int type);
	void OperTypeButtonSelect(int num);

private:
	const int						m_ButtonMtlTexBaseIndex = 0;
	shared_ptr<cRenderItem>			m_buttonRenderItem;
	cButtonCollector				m_operSelectButtons;
	cButtonCollector				m_drawButtons;
	cButtonCollector				m_meshButtons;
	cButtonCollector*				m_currButtons;

	vector<unique_ptr<cOperation>>	m_operations;
	cOperation*						m_currOperation;

	cDrawPlane							m_planes;
	cDrawMesh							m_meshs;
	cMesh*								m_currMesh;
	unordered_map<wstring, DrawItems>	m_drawItems;
	DrawItems*							m_currDraws;
};

