#include "stdafx.h"


cOper_Add_Plane::cOper_Add_Plane()
	: cOperation(OPER_ADD_PLANE)
{

}


cOper_Add_Plane::~cOper_Add_Plane()
{

}

void cOper_Add_Plane::DrawElementOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	assert(!draw.empty() && typeid(*draw[0].get()) == typeid(cDrawPlane) &&
		"this Container don`t have DrawPlane Element");

	static_cast<cDrawPlane*>(draw[0].get())->AddPlanes();
	m_operState = false;
}

void cOper_Add_Plane::CancleOperation()
{
}
