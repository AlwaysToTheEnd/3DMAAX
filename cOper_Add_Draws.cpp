#include "stdafx.h"


cOper_Add_Draws::cOper_Add_Draws()
	:cOperation(OPER_DRAWOPER)
	, m_selectDrawsIndex(-1)
{
}


cOper_Add_Draws::~cOper_Add_Draws()
{
}

void cOper_Add_Draws::Build()
{
	cOperation::Build();
	m_operationText->printString = L"Select Plane or Draws";

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

UINT cOper_Add_Draws::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	switch (m_worksSate)
	{
	case cOper_Add_Draws::SETUI:
	{
		m_operationText->isRender = true;
		m_operControl->ClearParameters();
		m_selectDrawsIndex = -1;

		for (auto& it : drawItems)
		{
			m_operControl->AddParameter(it.first, OPERDATATYPE_INDEX, &m_selectDrawsIndex);
		}

		UIMG->UIOn(m_operControl);
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
					CAMERA.SetTargetAndSettingAngle(it.second.m_plane);
					currDrawItems = &it.second;
					return 0;
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
				currDrawItems = &drawItems[name];
				return 0;
			}
		}
	}
		break;
	}

	return 0;
}


void cOper_Add_Draws::CancleOperation(DrawItems* draw)
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
