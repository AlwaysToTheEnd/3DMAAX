#include "stdafx.h"


cOper_Push_Mesh::cOper_Push_Mesh()
	: cOperation(OPER_PUSH_MESH)
	, m_currMesh(nullptr)
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

void cOper_Push_Mesh::MeshOperation(cDrawMesh & drawMesh, DrawItems * drawItem, cMesh ** currMesh)
{
	switch (m_worksSate)
	{
	case cOper_Push_Mesh::ADD_MESH_QUERY:
	{
		m_draws = nullptr;
		m_selectDrawsIndex = -1;

		if (drawItem && !currMesh)
		{
			m_currMesh = static_cast<cMesh*>(drawMesh.AddElement());
			*currMesh = m_currMesh;
			m_currMesh->SetDrawItems(drawItem);
			
			vector<DrawItems*>& meshDraws = m_currMesh->GetDraws();
			int i = 0;
			for (auto& it : meshDraws)
			{
				m_operControl.AddParameter(L"Draws"+to_wstring(i) , DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
				i++;
			}

			m_worksSate = DRAW_SELECT;
		}
		else if (currMesh)
		{
			m_currMesh = *currMesh;

			if (drawItem)
			{
				m_currMesh->SetDrawItems(drawItem);
				m_draws = drawItem;
			}

			m_worksSate = VERTEX_CHECK;
		}
		else
		{
			assert(false);
		}
	}
		break;
	case cOper_Push_Mesh::DRAW_SELECT:
	{
		m_operControl.Update(XMMatrixIdentity());

		if (m_selectDrawsIndex != -1)
		{
			m_draws = m_currMesh->GetDraws()[m_selectDrawsIndex];
			m_operControl.IsRenderState(false);
			m_worksSate = VERTEX_CHECK;
		}
	}
	break;
	case cOper_Push_Mesh::VERTEX_CHECK:
	{

	}
		break;
	}

	
}

DrawItems * cOper_Push_Mesh::MeshSelectOperation(unordered_map<wstring, DrawItems>& drawItems, cDrawMesh & drawMesh, cMesh ** mesh)
{
	return nullptr;
}

void cOper_Push_Mesh::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
}

void cOper_Push_Mesh::EndOperation()
{
}
