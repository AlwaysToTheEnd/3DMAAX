#include "stdafx.h"

cOper_Add_Mesh::cOper_Add_Mesh()
	: cOperation(OPER_MESHOPER)
	, m_selectDrawsIndex(-1)
{

}


cOper_Add_Mesh::~cOper_Add_Mesh()
{
}

UINT cOper_Add_Mesh::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	if (currMesh == nullptr)
	{
		if (meshs.empty())
		{
			currMesh = &meshs[L"mesh_0"];
			currMesh->Build(nullptr);
		}

		EndOperation();
	}

	//switch (m_worksSate)
	//{
	//case cOper_Add_Mesh::SETUI:
	//{
	//	if (currMesh == nullptr)
	//	{
	//		if (meshs.empty())
	//		{
	//			meshs.insert({ L"mesh_0",{} });
	//			currMesh = &meshs[L"mesh_0"];
	//		}
	//	}

	//	m_operationText->isRender = true;
	//	m_operControl->ClearParameters();
	//	m_selectDrawsIndex = -1;

	//	for (auto& it : drawItems)
	//	{
	//		m_operControl->AddParameter(it.first, DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
	//	}

	//	m_operControl->SetRenderState(true);
	//	m_worksSate = cOper_Add_Mesh::PICK;
	//}
	//break;
	//case cOper_Add_Mesh::PICK:
	//{
	//	if (m_selectDrawsIndex != -1)
	//	{
	//		int i = 0;
	//		for (auto& it : drawItems)
	//		{
	//			if (m_selectDrawsIndex == i)
	//			{
	//				(*mesh)->SetDrawItems(&it.second);
	//				cOperation::EndOperation();
	//				return &it.second;
	//			}

	//			i++;
	//		}
	//	}
	//	/*else
	//	{
	//		cObject* pickMesh = nullptr;
	//		if (drawMesh.Picking(&pickMesh))
	//		{
	//			*mesh =static_cast<cMesh*>(pickMesh);
	//			cOperation::EndOperation();
	//			return nullptr;
	//		}
	//	}*/

	//	m_operControl->Update(XMMatrixIdentity());
	//}
	//break;
	//}

	return 0;
}

void cOper_Add_Mesh::CancleOperation(DrawItems* draw)
{
	cOperation::EndOperation();
}
