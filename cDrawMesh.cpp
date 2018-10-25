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


cDrawMesh::cDrawMesh()
{

}


cDrawMesh::~cDrawMesh()
{
}

void cDrawMesh::SetRenderItem(shared_ptr<cRenderItem> renderItem)
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


