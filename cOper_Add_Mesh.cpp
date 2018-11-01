#include "stdafx.h"

cOper_Add_Mesh::cOper_Add_Mesh()
	: cOperation(OPER_MESHOPER)
	, m_selectDrawsIndex(-1)
{

}


cOper_Add_Mesh::~cOper_Add_Mesh()
{
}

void cOper_Add_Mesh::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.SetPos({ 730,100,0 });

	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Select Draws";
	m_operationText->pos = { 30,30,0 };
}

DrawItems * cOper_Add_Mesh::MeshSelectOperation(
	unordered_map<wstring, DrawItems>& drawItems, cDrawMesh& drawMesh, cMesh** mesh)
{
	switch (m_worksSate)
	{
	case cOper_Add_Mesh::SETUI:
	{
		if (*mesh == nullptr)
		{
			*mesh = static_cast<cMesh*>(drawMesh.AddElement());
		}

		m_operationText->isRender = true;
		m_operControl.ClearParameters();
		m_selectDrawsIndex = -1;

		for (auto& it : drawItems)
		{
			m_operControl.AddParameter(it.first, DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
		}

		m_operControl.SetRenderState(true);
		m_worksSate = cOper_Add_Mesh::PICK;
	}
	break;
	case cOper_Add_Mesh::PICK:
	{
		if (m_selectDrawsIndex != -1)
		{
			int i = 0;
			for (auto& it : drawItems)
			{
				if (m_selectDrawsIndex == i)
				{
					(*mesh)->SetDrawItems(&it.second);
					cOperation::EndOperation();
					return &it.second;
				}

				i++;
			}
		}
		/*else
		{
			cObject* pickMesh = nullptr;
			if (drawMesh.Picking(&pickMesh))
			{
				*mesh =static_cast<cMesh*>(pickMesh);
				cOperation::EndOperation();
				return nullptr;
			}
		}*/

		m_operControl.Update(XMMatrixIdentity());
	}
	break;
	}

	return nullptr;
}

void cOper_Add_Mesh::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	cOperation::EndOperation();
}
