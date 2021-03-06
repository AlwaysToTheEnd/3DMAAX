#include "stdafx.h"

cUIOperWindow*			cOperation::m_operControl = nullptr;
shared_ptr<cRenderItem> cOperation::m_prevViewRender = nullptr;
shared_ptr<RenderInstance> cOperation::m_prevViewRenderInstance = nullptr;
MeshGeometry* cOperation::m_previewGeo = nullptr;

void CycleLine::CycleLineCheck(list<cLine*>& leaveLines,
	cLine * endLinkLine, int endLinkPoint, list<vector<const cDot*>>& cycleDots)
{
	int endcheck = endLinkLine->CheckLinked(line);

	switch (endcheck)
	{
	case 0:
	case 1:
		if (endcheck != endLinkPoint)
		{
			cycleDots.push_back(GetDotsToParents());
			return;
		}
	}

	for (auto it = leaveLines.begin(); it != leaveLines.end(); )
	{
		bool isNextLine = true;
		switch (line->CheckLinked(*it))
		{
		case 0:
		case 1:
			isNextLine = false;
			nextLines.push_back({ *it,this });
			break;
		}

		if (isNextLine)
		{
			it++;
		}
		else
		{
			it = leaveLines.erase(it);
		}
	}

	for (auto& it : nextLines)
	{
		it.CycleLineCheck(leaveLines, endLinkLine, endLinkPoint, cycleDots);
	}
}

vector<const cDot*> CycleLine::GetDotsToParents()
{
	vector<const cDot*> dots;

	CycleLine* currLine = this;
	while (true)
	{
		if (currLine->parentLine == nullptr)
		{
			break;
		}

		switch (currLine->line->CheckLinked(currLine->parentLine->line))
		{
		case 0:
			if (dots.empty())
			{
				dots.push_back(currLine->line->GetSecondDot());
			}

			dots.push_back(currLine->line->GetFirstDot());
			break;
		case 1:
			if (dots.empty())
			{
				dots.push_back(currLine->line->GetFirstDot());
			}

			dots.push_back(currLine->line->GetSecondDot());
			break;
		default:
			assert(false);
			break;
		}

		currLine = currLine->parentLine;
	}

	assert(dots.size());

	vector<const cDot*> result;

	while (dots.size())
	{
		result.push_back(dots.back());
		dots.pop_back();
	}

	return result;
}


void cOperation::OperationsBaseSetup()
{
	m_operControl = UIMG->AddUI<cUIOperWindow>("operationWindow");
	m_operControl->SetPos({ 550,50,0 });
	m_previewGeo = MESHMG->AddTemporaryMesh("previewGeo");
	m_prevViewRender = RENDERITEMMG->AddRenderItem(cMesh::m_meshRenderName);
	m_prevViewRender->SetGeometry(m_previewGeo, m_previewGeo->name);
	m_prevViewRenderInstance = m_prevViewRender->GetRenderIsntance();
	m_prevViewRenderInstance->m_isRenderOK = false;
	m_prevViewRender->SetRenderOK(false);
}

cOperation::cOperation(OPERATIONTYPE type)
	: m_operType(type)
	, m_operationText(nullptr)
	, m_operState(false)
	, m_worksSate(0)
	, m_currObjectControlParam(-2)
{
}

void cOperation::Build()
{
	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::OrangeRed;
	m_operationText->printString = L"";
	m_operationText->pos = { 20,50,0 };
}

void cOperation::EndOperation()
{
	m_operState = false;
	UIMG->UIOff(m_operControl);
	m_operControl->ClearParameters();
	m_prevViewRender->SetRenderOK(false);
	OBJCOORD->ObjectRegistration(nullptr);

	if (m_operationText)
	{
		m_operationText->isRender = false;
	}
	m_worksSate = 0;
}

void cOperation::StartOperation()
{
	m_operState = true; 
	m_operControl->ClearParameters();
}

bool cOperation::CurrDrawCheckAndPick(unordered_map<wstring, DrawItems>& drawItems, DrawItems *& currDrawItems)
{
	if (currDrawItems != nullptr)
	{
		currDrawItems->SetPickRender(2);
		m_currObjectControlParam = -2;
		return true;
	}

	switch (m_currObjectControlParam)
	{
	case -2:
	{
		m_operControl->ClearParameters();

		for (auto& it : drawItems)
		{
			m_operControl->AddParameter(it.first, OPERDATATYPE_INDEX, &m_currObjectControlParam);
		}

		m_operationText->printString = L"그림 오브젝트 선택";
		m_operationText->isRender = true;
		UIMG->UIOn(m_operControl);
		m_currObjectControlParam = -1;
	}
	break;
	case -1:
	{
	}
	break;
	default:
	{
		auto it = drawItems.begin();
		for (int i = 0; i < m_currObjectControlParam; i++)
		{
			it++;
		}

		currDrawItems = &it->second;
		it->second.SetPickRender(2);
		m_operationText->isRender = false;
		m_operControl->ClearParameters();
		m_currObjectControlParam = -2;
		return true;
	}
	break;
	}

	return false;
}

bool cOperation::CurrMeshCheckAndPick(unordered_map<wstring, cMesh>& meshs, cMesh *& currMesh)
{
	if (currMesh != nullptr)
	{
		m_currObjectControlParam = -2;
		return true;
	}

	switch (m_currObjectControlParam)
	{
	case -2:
	{
		m_operControl->ClearParameters();

		for (auto& it : meshs)
		{
			m_operControl->AddParameter(it.first, OPERDATATYPE_INDEX, &m_currObjectControlParam);
		}

		m_operationText->printString = L"작업할 메쉬 선택";
		m_operationText->isRender = true;
		UIMG->UIOn(m_operControl);
		m_currObjectControlParam = -1;
	}
	break;
	case -1:
	{

	}
	break;
	default:
	{
		auto it = meshs.begin();
		for (int i = 0; i < m_currObjectControlParam; i++)
		{
			it++;
		}

		currMesh = &it->second;
		m_operationText->isRender = false;
		m_operControl->ClearParameters();
		m_currObjectControlParam = -2;
		return true;
	}
	break;
	}

	return false;
}

list<vector<const cDot*>> cOperation::LineCycleCheck(DrawItems * drawItem)
{
	vector<cLine*> lines = drawItem->m_draws[DRAW_LINES]->GetObjectsPtr<cLine>();

	for (size_t i = 0; i < lines.size(); i++)
	{
		for (size_t j = 0; j < lines.size(); j++)
		{
			if (i == j) continue;

			switch (lines[i]->CheckLinked(lines[j]))
			{
			case -1:
				drawItem->m_draws[DRAW_LINES]->DeleteObject(lines[j--]);
				lines = drawItem->m_draws[DRAW_LINES]->GetObjectsPtr<cLine>();
				break;
			}
		}
	}

	size_t linesNum = lines.size();
	if (linesNum < 3)
	{
		return list<vector<const cDot*>>();
	}

	list<cLine*> leaveLines;
	list<vector<const cDot*>> cycleDots;

	for (size_t i = 0; i < lines.size(); i++)
	{
		leaveLines.clear();

		CycleLine rootLine;
		rootLine.line = lines[i];

		for (size_t j = i + 1; j < lines.size(); j++)
		{
			leaveLines.push_back(lines[j]);
		}

		for (int j = 0; j < i; j++)
		{
			leaveLines.push_back(lines[j]);
		}

		int nexSpotindex = -1;

		for (auto it = leaveLines.begin(); it != leaveLines.end(); )
		{
			bool addNextLine = false;
			switch (rootLine.line->CheckLinked(*it))
			{
			case 0:
			{
				if (nexSpotindex == -1)
				{
					nexSpotindex = 0;
				}

				if (nexSpotindex == 0)
				{
					addNextLine = true;
				}
			}
			break;
			case 1:
			{
				if (nexSpotindex == -1)
				{
					nexSpotindex = 1;
				}

				if (nexSpotindex == 1)
				{
					addNextLine = true;
				}
			}
			break;
			}

			if (addNextLine)
			{
				rootLine.nextLines.push_back({ *it, &rootLine });
				it = leaveLines.erase(it);
			}
			else
			{
				it++;
			}
		}

		if (rootLine.nextLines.empty()) continue;

		if (leaveLines.empty())
		{

		}

		for (auto& it : rootLine.nextLines)
		{
			it.CycleLineCheck(leaveLines, rootLine.line, nexSpotindex, cycleDots);
		}
	}

	OverLapCycleDotsCheck(cycleDots);

	return cycleDots;
}

void cOperation::OverLapCycleDotsCheck(list<vector<const cDot*>>& dotslist)
{
	for (auto it1 = dotslist.begin(); it1 != dotslist.end(); it1++)
	{
		vector<const cDot*> pivotDots = *it1;
		auto it2 = it1;
		it2++;
		for (it2; it2 != dotslist.end();)
		{
			if (it1->size() != it2->size())
			{
				it2++;
				continue;
			}

			vector<const cDot*>& dots = *it2;

			if (EqualCheck(pivotDots, dots))
			{
				it2 = dotslist.erase(it2);
			}
			else
			{
				it2++;
			}
		}
	}
}

bool cOperation::EqualCheck(vector<const cDot*>& lhs, vector<const cDot*>& rhs)
{
	for (int i = 0; i < lhs.size(); i++)
	{
		bool isEqual = false;
		for (int j = 0; j < lhs.size(); j++)
		{
			if (lhs[i] == rhs[j])
			{
				isEqual = true;
			}
		}

		if (isEqual == false)
		{
			return false;
		}
	}

	return true;
}

bool cOperation::CheckCWCycle(const vector<const cDot*>& cycle) const
{
	//http://www.gisdeveloper.co.kr/?p=805

	bool isRightCycle = false;

	float allAngle = 0;
	for (size_t i = 0; i < cycle.size() - 1; i++)
	{
		XMVECTOR currDot = XMLoadFloat3(&cycle[i]->GetPosC());
		XMVECTOR nextDot = XMLoadFloat3(&cycle[i + 1]->GetPosC());

		float x0 = currDot.m128_f32[0];
		float x1 = nextDot.m128_f32[0];
		float y0 = currDot.m128_f32[1];
		float y1 = nextDot.m128_f32[1];

		allAngle += (x0*y1) - (x1*y0);
	}

	return allAngle < 0;
}

XMVECTOR XM_CALLCONV cOperation::GetNormalFromTriangle(const XMFLOAT3 & pos1, const XMFLOAT3 & pos2, const XMFLOAT3 & pos3)
{
	XMVECTOR originVector = XMLoadFloat3(&pos1);
	XMVECTOR tryVector1 = XMLoadFloat3(&pos2) - originVector;
	XMVECTOR tryVector2 = XMLoadFloat3(&pos3) - originVector;

	return XMVector3Normalize(XMVector3Cross(tryVector1, tryVector2));
}

cDot * cOperation::AddDot(vector<unique_ptr<cDrawElement>>& draw)
{
	return static_cast<cDot*>(draw[DRAW_DOTS]->AddElement());
}

cLine* cOperation::AddLine(vector<unique_ptr<cDrawElement>>& draw)
{
	return static_cast<cLine*>(draw[DRAW_LINES]->AddElement());
}

bool cOperation::PickPlane(cDrawPlane* planes, cPlane ** plane)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		if (!m_operControl->IsMousePosInUI())
		{
			cObject* pickPlane = nullptr;
			if (planes->Picking(&pickPlane))
			{
				CAMERA.SetTargetAndSettingAngle(pickPlane);
				*plane = static_cast<cPlane*>(pickPlane);
				return true;
			}
		}
	}

	return false;
}

cDot * cOperation::AddDotAtCurrPlane(DrawItems* drawItems, bool* isAddDot)
{
	if (INPUTMG->GetMouseOneDown(VK_LBUTTON))
	{
		if (!m_operControl->IsMousePosInUI())
		{
			float distance;
			PICKRAY ray = INPUTMG->GetMousePickLay();
			if (drawItems->m_plane->Picking(ray, distance))
			{
				cObject* pickDot = nullptr;
				if (drawItems->m_draws[DRAW_DOTS]->Picking(&pickDot))
				{
					if (isAddDot)
					{
						*isAddDot = false;
					}
					cDot* resultDot = static_cast<cDot*>(pickDot);
					return resultDot;
				}

				if (isAddDot)
				{
					*isAddDot = true;
				}
				XMMATRIX planeInvMat = XMMatrixInverse(&XMVECTOR(), drawItems->m_plane->GetXMMatrix());
				XMVECTOR pos = ray.origin + ray.ray*distance;
				pos = XMVector3TransformCoord(pos, planeInvMat);
				pos.m128_f32[2] = 0;
				cDot* dot = AddDot(drawItems->m_draws);
				dot->SetHostObject(drawItems->m_plane);
				XMStoreFloat3(&dot->GetPos(), pos);

				return dot;
			}
		}
	}

	return nullptr;
}
