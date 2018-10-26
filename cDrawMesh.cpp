#include "stdafx.h"

const string cDrawMesh::m_meshRenderName = "baseMesh";

cMesh::cMesh()
	:m_geo(nullptr)
	, m_renderItem(nullptr)
{
}

cMesh::~cMesh()
{
}

void cMesh::Build(shared_ptr<cRenderItem> renderItem)
{
	m_renderItem = renderItem;
	m_renderInstance = m_renderItem->GetRenderIsntance();
}

void XM_CALLCONV cMesh::Update(FXMMATRIX mat)
{

}

bool XM_CALLCONV cMesh::Picking(PICKRAY ray, float & distance)
{
	return false;
}

bool cMesh::LineCycleCheck(UINT drawItemIndex)
{
	vector<cLine*> lines = m_draws[drawItemIndex]->m_draws[DRAW_LINES]->GetObjectsPtr<cLine>();

	for (size_t i = 0; i < lines.size(); i++)
	{
		for (size_t j = 0; j < lines.size(); j++)
		{
			if (i == j) continue;

			switch (lines[i]->CheckLinked(lines[j]))
			{
			case -1:
				m_draws[drawItemIndex]->m_draws[DRAW_LINES]->DeleteObject(lines[j--]);
				lines = m_draws[drawItemIndex]->m_draws[DRAW_LINES]->GetObjectsPtr<cLine>();
				break;
			}
		}
	}

	size_t linesNum = lines.size();
	if (linesNum < 3)
	{
		return false;
	}

	list<cLine*> leaveLines;
	list<vector<const cDot*>> cycleDots;

	for (size_t i = 0; i < lines.size(); i++)
	{
		leaveLines.clear();

		CycleLine rootLine;
		rootLine.line = lines[i];

		for (int j = 0; j < lines.size(); j++)
		{
			if (j == i) continue;

			leaveLines.push_back(lines[j]);
		}

		int nexSpotindex = -1;

		for (auto it = leaveLines.begin(); it != leaveLines.end(); )
		{
			bool isNextLine = true;
			switch (rootLine.line->CheckLinked(*it))
			{
			case 0:
			{
				if (nexSpotindex = -1)
				{
					nexSpotindex = 0;
				}

				if (nexSpotindex == 0)
				{
					isNextLine == false;
				}
			}
			break;
			case 1:
			{
				if (nexSpotindex = -1)
				{
					nexSpotindex = 1;
				}

				if (nexSpotindex == 1)
				{
					isNextLine == false;
				}
			}
			break;
			}

			if (isNextLine)
			{
				it++;
			}
			else
			{
				rootLine.nextLines.push_back({ *it, &rootLine });
				it = leaveLines.erase(it);
			}
		}

		if (rootLine.nextLines.empty()) continue;

		for (auto& it : rootLine.nextLines)
		{
			it.CycleLineCheck(leaveLines, rootLine.line, cycleDots);
		}
	}

	OverLapCycleDotsCheck(cycleDots);

}

void cMesh::OverLapCycleDotsCheck(list<vector<const cDot*>>& dotslist)
{
	for (auto it1 = dotslist.begin(); it1 != dotslist.end(); it1++)
	{
		vector<const cDot*> pivotDots = *it1;
		for (auto it2 = dotslist.begin(); it2 != dotslist.end();)
		{
			if (it1 == it2 || it1->size() != it2->size()) continue;

			vector<const cDot*>& dots = *it2;

			int equalStartIndex = -1;

			for (size_t i = 0; i < pivotDots.size(); i++)
			{
				if (equalStartIndex == -1)
				{
					if (pivotDots[0] == dots[i])
					{
						equalStartIndex = i;
					}
				}
				else
				{
					if (pivotDots[i - equalStartIndex] != dots[i])
					{
						it2 = dotslist.erase(it2);
						continue;
					}
				}
			}

			if (equalStartIndex == -1)
			{
				it2 = dotslist.erase(it2);
				continue;
			}

			for (size_t i = 0; i < equalStartIndex; i++)
			{
				if (pivotDots[equalStartIndex + i] != dots[i])
				{
					it2 = dotslist.erase(it2);
					continue;
				}
			}
		}
	}
}


void cMesh::CycleLine::CycleLineCheck(list<cLine*>& leaveLines,
	cLine * endLinkLine, list<vector<const cDot*>>& cycleDots)
{
	int endcheck = line->CheckLinked(endLinkLine);
	if (endcheck == 0 || endcheck == 1)
	{
		cycleDots.push_back(GetDotsToParents());
		return;
	}

	if (leaveLines.empty()) return;

	for (auto it = leaveLines.begin(); it != leaveLines.end(); )
	{
		bool isNextLine = true;
		switch (line->CheckLinked(*it))
		{
		case 0:
		case 1:
			isNextLine == false;
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
		it.CycleLineCheck(leaveLines, endLinkLine, cycleDots);
	}
}

vector<const cDot*> cMesh::CycleLine::GetDotsToParents()
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
	return dots;
}

UINT cMesh::SetDrawItems(DrawItems * drawItem)
{
	UINT indexNum = 0;
	for (auto& it : m_draws)
	{
		if (drawItem == it) return indexNum;
		indexNum++;
	}

	m_draws.push_back(drawItem);
	return indexNum;
}


cDrawMesh::cDrawMesh()
{

}


cDrawMesh::~cDrawMesh()
{
}

cObject * cDrawMesh::AddElement()
{
	auto renderItem = RENDERITEMMG->AddRenderItem(m_meshRenderName);
	auto geometry = MESHMG->AddTemporaryMesh(m_meshRenderName + to_string(m_objects.size()));
	renderItem->SetGeometry(geometry, m_meshRenderName + to_string(m_objects.size()));
	renderItem->SetRenderOK(false);

	auto addObejct = make_unique<cMesh>();
	addObejct->Build(renderItem);
	addObejct->SetGeometry(geometry);

	m_objects.push_back(move(addObejct));
	return m_objects.back().get();
}

