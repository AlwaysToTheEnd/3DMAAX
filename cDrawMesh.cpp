#include "stdafx.h"

const string cDrawMesh::m_meshRenderName = "baseMesh";

cMesh::cMesh()
	:m_geo(nullptr)
	,m_renderItem(nullptr)
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
	vector<cDot*> dots = m_draws[drawItemIndex]->m_draws[DRAW_DOTS]->GetObjectsPtr<cDot>();
	
	size_t dotsNum = dots.size();
	if (dotsNum < 3)
	{
		return false;
	}

	vector<cDot*> circleDots;

	for (auto& it: dots)
	{
		while (it)
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


