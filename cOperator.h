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
	bool OperationCheck();
	void ObjectListUIUpdate();

private:
	void OperationStart(int type);
	void SelectPlane(UINT64 index);
	void SelectDraws(UINT64 index);
	void SelectMeshs(UINT64 index);

private:
	const int						m_ButtonMtlTexBaseIndex = 0;
	shared_ptr<cRenderItem>			m_buttonRenderItem;
	cButtonCollector				m_operSelectButtons;

	vector<unique_ptr<cOperation>>	m_operations;
	cOperation*						m_currOperation;

	cDrawPlane							m_planes;
	unordered_map<wstring, cMesh>		m_mesh;
	unordered_map<wstring, DrawItems>	m_drawItems;
	DrawItems*							m_currDraws;
	cMesh*								m_currMesh;
	cUIOperWindow						m_operControl;
	cUIOperWindow						m_objectControl;
};

