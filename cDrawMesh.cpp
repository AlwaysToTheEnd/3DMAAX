#include "stdafx.h"


cDrawMesh::cDrawMesh()
{
}


cDrawMesh::~cDrawMesh()
{
}

void cDrawMesh::SetRenderItem(shared_ptr<cRenderItem> renderItem)
{
}

bool cDrawMesh::Picking(cObject ** ppObject)
{
	return false;
}

cObject * cDrawMesh::AddElement()
{
	return nullptr;
}

void cMesh::Build(shared_ptr<cRenderItem> renderItem)
{
}

void XM_CALLCONV cMesh::Update(FXMMATRIX mat)
{
	
}

bool XM_CALLCONV cMesh::Picking(PICKRAY ray, float & distance)
{
	return false;
}
