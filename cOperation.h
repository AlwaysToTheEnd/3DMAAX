#pragma once
#include "cUIObject.h"
#include "cUIOperWindow.h"
#include "cMesh.h"

enum OPERATIONTYPE
{
	OPER_ADD_PLANE,
	OPER_ADD_LINE,
	OPER_ADD_DOT,
	OPER_DRAWOPER,
	OPER_PUSH_MESH,
	OPER_MESHOPER,
	OPER_COUNT,
};

struct CycleLine
{
	cLine* line = nullptr;
	CycleLine* parentLine = nullptr;

	vector<CycleLine> nextLines;

	CycleLine() {}
	CycleLine(cLine* line, CycleLine* parentLine)
	{
		this->line = line;
		this->parentLine = parentLine;
	}

	void CycleLineCheck(list<cLine*>& leaveLines,
		cLine * endLinkLine, int endLinkPoint, list<vector<const cDot*>>& cycleDots);

	vector<const cDot*> GetDotsToParents();
};

class cOperation
{
public:
	static void OperationsBaseSetup(shared_ptr<cRenderItem> renderItem);

public:
	cOperation(OPERATIONTYPE type)
		: m_operType(type)
		, m_operationText(nullptr)
		, m_operState(false)
		, m_worksSate(0)
		, m_currObjectControlParam(-2)
	{

	}

	virtual ~cOperation() {}
	virtual void SetUp();
	virtual UINT OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
		cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh) { assert(false); return 0; }

	virtual void CancleOperation(DrawItems* draw) = 0;
	virtual void EndOperation();

public:
	void StartOperation() { m_operState = true; m_operControl.SetRenderState(true); }

	bool GetOperState() const { return m_operState; }
	OPERATIONTYPE GetOperType() const { return m_operType; }

protected:
	bool CurrDrawCheckAndPick(unordered_map<wstring, DrawItems>& drawItems, DrawItems*& currDrawItems);
	cDot* AddDot(vector<unique_ptr<cDrawElement>>& draw);
	cLine* AddLine(vector<unique_ptr<cDrawElement>>& draw);
	bool PickPlane(cDrawPlane* planes, cPlane** plane);
	cDot* AddDotAtCurrPlane(DrawItems* drawItems);

protected:
	bool CurrMeshCheckAndPick(unordered_map<wstring, cMesh>& meshs, cMesh*& currMesh);
	list<vector<const cDot*>> LineCycleCheck(DrawItems* drawItem);
	void OverLapCycleDotsCheck(list<vector<const cDot*>>& dotslist);
	bool EqualCheck(vector<const cDot*>& lhs, vector<const cDot*>& rhs);
	bool CheckCWCycle(vector<const cDot*>& cycle);
	XMVECTOR XM_CALLCONV GetNormalFromTriangle(const XMFLOAT3& pos1, const XMFLOAT3& pos2, const XMFLOAT3& pos3);

protected:
	static shared_ptr<cRenderItem>		m_OperatorUi;
	static shared_ptr<cRenderItem>		m_prevViewRender;
	static shared_ptr<RenderInstance>	m_prevViewRenderInstance;
	static MeshGeometry*	m_previewGeo;

	shared_ptr<RenderFont>	m_operationText;
	const OPERATIONTYPE		m_operType;
	cUIOperWindow			m_operControl;
	bool					m_operState;
	UINT					m_worksSate;
	int						m_currObjectControlParam;
};