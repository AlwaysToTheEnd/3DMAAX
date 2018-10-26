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

bool cMesh::VertexCircleCheck(UINT drawItemIndex)
{
	vector<cLine*> lines = m_draws[drawItemIndex]->m_draws[DRAW_LINES]->GetObjectsPtr<cLine>();

	size_t linesNum = lines.size();
	if (linesNum < 3)
	{
		return false;
	}

	vector<const cDot*> circleDots;

	for (size_t i = 0; i < lines.size(); i++)
	{
		vector<cLine*> checkLines;
		const cDot* endLinkDot = nullptr;
		const cDot* nextPivotDot = nullptr;

		for (size_t j = 0; j < lines.size(); j++)
		{
			if (i == j) continue;

			switch (lines[i]->CheckLinked(lines[j]))
			{
			case -1:
				m_draws[drawItemIndex]->m_draws[DRAW_LINES]->DeleteObject(lines[j--]);
				lines = m_draws[drawItemIndex]->m_draws[DRAW_LINES]->GetObjectsPtr<cLine>();
				break;
			case 0:
				if (endLinkDot == nullptr)
				{
					endLinkDot = lines[i]->GetSecondDot();
					checkLines.push_back(lines[j]);
					nextPivotDot = lines[i]->GetFirstDot();
				}
				else
				{
					if (nextPivotDot == lines[i]->GetFirstDot())
					{
						checkLines.push_back(lines[j]);
					}
				}
				break;
			case 1:
				if (endLinkDot == nullptr)
				{
					endLinkDot = lines[i]->GetFirstDot();
					checkLines.push_back(lines[j]);
					nextPivotDot = lines[i]->GetSecondDot();
				}
				else
				{
					if (nextPivotDot == lines[i]->GetSecondDot())
					{
						checkLines.push_back(lines[j]);
					}
				}
				break;
			}
		}

		for (size_t j = 0; j < checkLines.size(); j++)
		{

		}
	}
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
	renderItem->SetGeometry(geometry, m_meshRenderName);
	renderItem->SetRenderOK(false);

	auto addObejct = make_unique<cMesh>();
	addObejct->Build(renderItem);
	addObejct->SetGeometry(geometry);

	m_objects.push_back(move(addObejct));
	return m_objects.back().get();
}


