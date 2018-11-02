#include "stdafx.h"

const string cDrawMesh::m_meshRenderName = "baseMesh";

cMesh::cMesh()
	: m_geo(nullptr)
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

list<vector<const cDot*>> cMesh::LineCycleCheck(UINT drawItemIndex)
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

void cMesh::SubObjectSubAbsorption()
{
	m_rootCSG.SubObjectSubAbsorption();
	m_rootCSG.GetData(m_vertices, m_indices);

	MESHMG->ChangeMeshGeometryData(m_geo->name, m_vertices.data(), m_indices.data(), sizeof(NT_Vertex),
		sizeof(NT_Vertex)*(UINT)m_vertices.size(), DXGI_FORMAT_R32_UINT, sizeof(UINT)*(UINT)m_indices.size(),
		true);

	m_geo->octree = nullptr;
	m_geo->SetOctree(0);
	m_renderItem->SetGeometry(m_geo, m_geo->name);
	m_renderItem->SetRenderOK(true);
}

void cMesh::AddCSGObject(CSGWORKTYPE work, unique_ptr<cCSGObject> object)
{
	m_rootCSG.AddChild(work, move(object));
}

bool XM_CALLCONV cMesh::GetPickTriangleInfo(PICKRAY ray, FXMVECTOR baseDir, float& dist, XMFLOAT4 * quaternion)
{
	XMFLOAT3 pos0, pos1, pos2;

	if (m_geo->octree->GetPickTriangle(ray, dist, pos0, pos1, pos2))
	{
		XMVECTOR _pos0 = XMLoadFloat3(&pos0);
		XMVECTOR _pos1 = XMLoadFloat3(&pos1);
		XMVECTOR _pos2 = XMLoadFloat3(&pos2);

		if (quaternion)
		{
			XMVECTOR v1 = _pos1 - _pos0;
			XMVECTOR v2 = _pos2 - _pos0;
			XMVECTOR triangleNormal = XMVector3Cross(v1, v2);
			XMVECTOR frontDir = baseDir;
			XMVECTOR quaternionAxis = XMVector3Cross(frontDir, triangleNormal);
			
			float angle = 0;
			XMStoreFloat(&angle, XMVector3AngleBetweenVectors(frontDir, triangleNormal));

			if (XMVector3Equal(quaternionAxis, XMVectorZero()))
			{
				XMMATRIX rotationMat = XMMatrixRotationX(XM_PIDIV2);
				quaternionAxis = XMVector3TransformNormal(baseDir, rotationMat);
			}

			XMVECTOR TriangleQuaternion = XMQuaternionRotationAxis(quaternionAxis, angle);
			XMStoreFloat4(quaternion, TriangleQuaternion);
		}

		return true;
	}

	return false;
}

void cMesh::OverLapCycleDotsCheck(list<vector<const cDot*>>& dotslist)
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

bool cMesh::EqualCheck(vector<const cDot*>& lhs, vector<const cDot*>& rhs)
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


void cMesh::CycleLine::CycleLineCheck(list<cLine*>& leaveLines,
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
		}
		break;
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

	vector<const cDot*> result;

	while (dots.size())
	{
		result.push_back(dots.back());
		dots.pop_back();
	}

	return result;
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

