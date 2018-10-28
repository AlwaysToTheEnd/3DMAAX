#include "stdafx.h"


cOper_Push_Mesh::cOper_Push_Mesh()
	: cOperation(OPER_PUSH_MESH)
	, m_draws(nullptr)
	, m_selectDrawsIndex(-1)
{

}


cOper_Push_Mesh::~cOper_Push_Mesh()
{
}

void cOper_Push_Mesh::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.SetPos({ 730,100,0 });

	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Select Draws";
	m_operationText->pos = { 30,30,0 };
}

void cOper_Push_Mesh::MeshOperation(cMesh* currMesh)
{
	switch (m_worksSate)
	{
	case cOper_Push_Mesh::ADD_MESH_QUERY:
	{
		if (currMesh == nullptr)
		{
			EndOperation();
			return;
		}

		m_draws = nullptr;
		m_selectDrawsIndex = -1;

		vector<DrawItems*>& meshDraws = currMesh->GetDraws();
		int i = 0;
		for (auto& it : meshDraws)
		{
			m_operControl.AddParameter(L"Draws" + to_wstring(i), DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
			i++;
		}

		if (i == 0)
		{
			EndOperation();
			return;
		}

		m_operControl.IsRenderState(true);
		m_worksSate = DRAW_SELECT;
	}
	break;
	case cOper_Push_Mesh::DRAW_SELECT:
	{
		m_operControl.Update(XMMatrixIdentity());

		if (m_selectDrawsIndex != -1)
		{
			m_draws = currMesh->GetDraws()[m_selectDrawsIndex];
			m_operControl.IsRenderState(false);
			m_currDrawCycleDotsList = currMesh->LineCycleCheck(m_selectDrawsIndex);

			if (m_currDrawCycleDotsList.empty())
			{
				m_selectDrawsIndex = -1;
			}
			else
			{
				m_worksSate = CYCLE_SELECT;
			}
		}
	}
	break;
	case cOper_Push_Mesh::CYCLE_SELECT:
	{


	}
	break;
	}


}

void cOper_Push_Mesh::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	EndOperation();
}
