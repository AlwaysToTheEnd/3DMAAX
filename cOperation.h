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
	static void OperationsBaseSetup();

public:
	cOperation(OPERATIONTYPE type);
	virtual ~cOperation() {}
	virtual void Build();
	virtual UINT OperationUpdate(	unordered_map<wstring, DrawItems>& drawItems,
									cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, 
									DrawItems*& currDrawItems, cMesh*& currMesh) { assert(false); return 0; }

	virtual void CancleOperation(DrawItems* draw) = 0;
	virtual void EndOperation();

public:
	void StartOperation(); 

	bool GetOperState() const { return m_operState; }
	OPERATIONTYPE GetOperType() const { return m_operType; }

protected:
	bool CurrDrawCheckAndPick(unordered_map<wstring, DrawItems>& drawItems, DrawItems*& currDrawItems);
	bool CurrMeshCheckAndPick(unordered_map<wstring, cMesh>& meshs, cMesh*& currMesh);

	cDot* AddDot(vector<unique_ptr<cDrawElement>>& draw);
	cLine* AddLine(vector<unique_ptr<cDrawElement>>& draw);
	bool PickPlane(cDrawPlane* planes, cPlane** plane);
	cDot* AddDotAtCurrPlane(DrawItems* drawItems, bool* isAddDot=nullptr);

protected:
	list<vector<const cDot*>>	LineCycleCheck(DrawItems* drawItem);
	void						OverLapCycleDotsCheck(list<vector<const cDot*>>& dotslist);
	bool						EqualCheck(vector<const cDot*>& lhs, vector<const cDot*>& rhs);

	template<typename vertexType, typename indexType>
	void EarClipping(const vector<vertexType>& dots, vector<indexType>& indices);
	bool CheckCWCycle(const vector<const cDot*>& cycle) const;

protected:
	XMVECTOR XM_CALLCONV GetNormalFromTriangle(const XMFLOAT3& pos1, const XMFLOAT3& pos2, const XMFLOAT3& pos3);
	
protected:
	static cUIOperWindow*				m_operControl;
	static shared_ptr<cRenderItem>		m_prevViewRender;
	static shared_ptr<RenderInstance>	m_prevViewRenderInstance;
	static MeshGeometry*				m_previewGeo;

	shared_ptr<RenderFont>	m_operationText;
	shared_ptr<RenderFont>	m_operationText_K;
	const OPERATIONTYPE		m_operType;
	bool					m_operState;
	UINT					m_worksSate;
	int						m_currObjectControlParam;
};


template<typename vertexType, typename indexType>
inline void cOperation::EarClipping(const vector<vertexType>& dots, vector<indexType>& indices)
{
	vector<UINT> earClipingIndex(dots.size());
	iota(earClipingIndex.begin(), earClipingIndex.end(), 0);

	UINT currIndex = 0;
	while (true)
	{
		UINT firstDotsIndex = earClipingIndex[currIndex];
		UINT nextDotIndex1 = earClipingIndex[(currIndex + 1) % earClipingIndex.size()];
		UINT nextDotIndex2 = earClipingIndex[(currIndex + 2) % earClipingIndex.size()];

		XMVECTOR originVector = XMLoadFloat3(&dots[firstDotsIndex].pos);
		XMVECTOR tryVector1 = XMLoadFloat3(&dots[nextDotIndex1].pos) - originVector;
		XMVECTOR tryVector2 = XMLoadFloat3(&dots[nextDotIndex2].pos) - originVector;

		XMVECTOR crossVector = XMVector3Cross(tryVector1, tryVector2);

		if (crossVector.m128_f32[2] > 0)
		{
			//dot in triangle check

			bool isInsertDot = false;
			float tryVector1Length = XMVector3Length(tryVector1).m128_f32[0];
			float tryVector2Length = XMVector3Length(tryVector2).m128_f32[0];
			XMVECTOR normaTryVector1 = XMVector3Normalize(tryVector1);
			XMVECTOR normaTryVector2 = XMVector3Normalize(tryVector2);

			for (size_t i = 0; i < dots.size(); i++)
			{
				if (i != firstDotsIndex && i != nextDotIndex1 && i != nextDotIndex2)
				{
					XMVECTOR insertCheckVector = XMLoadFloat3(&dots[i].pos) - originVector;
					float try1dot;
					float try2dot;
					XMStoreFloat(&try1dot, XMVector3Dot(insertCheckVector, normaTryVector1));
					XMStoreFloat(&try2dot, XMVector3Dot(insertCheckVector, normaTryVector2));
					float u = try1dot / tryVector1Length;
					float v = try2dot / tryVector2Length;

					if (u >= 0 && v >= 0 && u <= 1.0f && v <= 1.0f)
					{
						if (XMVector3Cross(tryVector1, insertCheckVector).m128_f32[2] > 0 &&
							XMVector3Cross(tryVector2, insertCheckVector).m128_f32[2] < 0)
						{
							isInsertDot = true;
							break;
						}
					}
				}
			}

			if (!isInsertDot)
			{
				indices.push_back(firstDotsIndex);
				indices.push_back(nextDotIndex1);
				indices.push_back(nextDotIndex2);

				auto it = earClipingIndex.begin();
				for (int i = 0; i < (currIndex + 1) % earClipingIndex.size(); i++)
				{
					it++;
				}

				earClipingIndex.erase(it);

				if (earClipingIndex.size() == currIndex)
				{
					currIndex--;
				}
			}
			else
			{
				currIndex = (currIndex + 1) % earClipingIndex.size();
			}
		}
		else
		{
			currIndex = (currIndex + 1) % earClipingIndex.size();
		}

		if (earClipingIndex.size() <= 3)
		{
			if (earClipingIndex.size() == 3)
			{
				originVector = XMLoadFloat3(&dots[earClipingIndex[0]].pos);
				tryVector1 = XMLoadFloat3(&dots[earClipingIndex[1]].pos) - originVector;
				tryVector2 = XMLoadFloat3(&dots[earClipingIndex[2]].pos) - originVector;

				crossVector = XMVector3Cross(tryVector1, tryVector2);

				if (crossVector.m128_f32[2] < 0)
				{
					indices.push_back(earClipingIndex[0]);
					indices.push_back(earClipingIndex[2]);
					indices.push_back(earClipingIndex[1]);
				}
				else
				{
					indices.push_back(earClipingIndex[0]);
					indices.push_back(earClipingIndex[1]);
					indices.push_back(earClipingIndex[2]);
				}
			}

			break;
		}
	}
}
