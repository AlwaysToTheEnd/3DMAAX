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

DrawItems * cOper_Add_Mesh::DrawSelectOperation(unordered_map<wstring, DrawItems>& drawItems)
{
	switch (m_worksSate)
	{
	case cOper_Add_Mesh::SETUI:
	{
		m_operationText->isRender = true;
		m_operControl.ClearParameters();
		m_selectDrawsIndex = -1;

		for (auto& it : drawItems)
		{
			m_operControl.AddParameter(it.first, DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
		}

		m_operControl.IsRenderState(true);
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
					cOperation::EndOperation();
					return &it.second;
				}

				i++;
			}
		}

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
