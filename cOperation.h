#pragma once
#include "cUIObject.h"
#include "cUIOperWindow.h"
#include "cDrawMesh.h"


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

class cPlane;

struct DrawItems
{
	vector<unique_ptr<cDrawElement>>	m_draws;
	cPlane*								m_plane = nullptr;

	DrawItems() {}
	DrawItems(cPlane* plane)
	{
		m_plane = plane;
	}

	/*DrawItems(DrawItems& rhs)
	{
		m_draws.resize(rhs.m_draws.size());
	
		for (size_t i = 0; i < m_draws.size(); i++)
		{
			m_draws[i] = move(rhs.m_draws[i]);
		}

		m_plane = rhs.m_plane;

		rhs.m_draws.clear();
		rhs.m_plane = nullptr;
	}*/

	void SetPickRender(UINT key)
	{
		for (auto& it : m_draws)
		{
			it->SetPickRender(key);
		}
	}
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
	{

	}

	virtual ~cOperation() {}
	virtual void SetUp() = 0;
	virtual void DrawElementOperation(DrawItems* drawItems) { assert(false); }
	virtual void PlaneAddOperation(cDrawPlane& planes) { assert(false); }
	virtual void MeshOperation(cMesh* currMesh) { assert(false); }
	virtual DrawItems* DrawsAddOperatioin(unordered_map<wstring, DrawItems>& drawItems, cDrawPlane& planes) { assert(false); return nullptr; }
	virtual DrawItems* MeshSelectOperation(unordered_map<wstring, DrawItems>& drawItems, cDrawMesh& drawMesh, cMesh** mesh) { assert(false); return nullptr; }

	virtual void CancleOperation(vector<unique_ptr<cDrawElement>>& draw) = 0;
	virtual void EndOperation();

public:
	void StartOperation() { m_operState = true; m_operControl.IsRenderState(true); }

	bool GetOperState() const { return m_operState; }
	OPERATIONTYPE GetOperType() const { return m_operType; }

protected:
	cDot* AddDot(vector<unique_ptr<cDrawElement>>& draw);
	cLine* AddLine(vector<unique_ptr<cDrawElement>>& draw);
	bool PickPlane(cDrawPlane* planes, cPlane** plane);
	cDot * AddDotAtCurrPlane(DrawItems* drawItems);

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
};