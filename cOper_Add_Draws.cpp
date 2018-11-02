#include "stdafx.h"


cOper_Add_Draws::cOper_Add_Draws()
	:cOperation(OPER_DRAWOPER)
	, m_selectDrawsIndex(-1)
{
}


cOper_Add_Draws::~cOper_Add_Draws()
{
}

void cOper_Add_Draws::SetUp()
{
	m_operControl.Build(m_OperatorUi);
	m_operControl.SetPos({ 730,100,0 });

	m_operationText = FONTMANAGER->GetFont("baseFont");
	m_operationText->isRender = false;
	m_operationText->color = Colors::Red;
	m_operationText->printString = L"Select Plane or Draws";
	m_operationText->pos = { 30,30,0 };

	for (int i = 0; i < DRAWOBJECTSPLACE::OBJECTS_COUNT; i++)
	{
		switch (i)
		{
		case DRAW_LINES:
			m_renderItems.push_back(RENDERITEMMG->AddRenderItem("line"));
			break;
		case DRAW_DOTS:
			m_renderItems.push_back(RENDERITEMMG->AddRenderItem("dot"));
			break;
		case OBJECTS_COUNT:
			break;
		default:
			assert(false);
			break;
		}
	}
}

DrawItems* cOper_Add_Draws::DrawsAddOperatioin(unordered_map<wstring, DrawItems>& drawItems, cDrawPlane & planes)
{
	switch (m_worksSate)
	{
	case cOper_Add_Draws::SETUI:
	{
		m_operationText->isRender = true;
		m_operControl.ClearParameters();
		m_selectDrawsIndex = -1;

		for (auto& it : drawItems)
		{
			m_operControl.AddParameter(it.first, DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
		}

		m_operControl.SetRenderState(true);
		m_worksSate = cOper_Add_Draws::PICK;
	}
		break;
	case cOper_Add_Draws::PICK:
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
		else
		{
			cPlane* plane = nullptr;

			if (PickPlane(&planes, &plane))
			{
				wstring name = L"drawItems" + to_wstring(drawItems.size());
				drawItems.insert({ name, DrawItems(plane) });
				SetDraws(drawItems[name]);

				cOperation::EndOperation();
				return &drawItems[name];
			}
		}

		m_operControl.Update(XMMatrixIdentity());
	}
		break;
	}

	return nullptr;
}


void cOper_Add_Draws::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
{
	cOperation::EndOperation();
}

void cOper_Add_Draws::SetDraws(DrawItems & drawItems)
{
	for (int i = 0; i < DRAWOBJECTSPLACE::OBJECTS_COUNT; i++)
	{
		switch (i)
		{
		case DRAW_LINES:
			drawItems.m_draws.push_back(unique_ptr<cDrawElement>(new cDrawLines));
			drawItems.m_draws.back()->SetRenderItem(m_renderItems[i]);
			break;
		case DRAW_DOTS:
			drawItems.m_draws.push_back(unique_ptr<cDrawElement>(new cDrawDot));
			drawItems.m_draws.back()->SetRenderItem(m_renderItems[i]);
			break;
		default:
			assert(false);
			break;
		}
	}

	drawItems.SetRenderState(true);
}
