#include "stdafx.h"

cDrawPlane::cDrawPlane()
	: m_plane(Xmfloat4Zero())
{
}

cDrawPlane::cDrawPlane(XMFLOAT4 plane)
	: m_plane(plane)
{
}

cDrawPlane::~cDrawPlane()
{

}

void cDrawPlane::SetPlane(XMVECTOR plane)
{

}

bool XM_CALLCONV cDrawPlane::Picking(PICKRAY ray, float & distance)
{


	return false;
}
