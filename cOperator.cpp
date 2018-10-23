#include "stdafx.h"

cOperator::cOperator()
	: m_currOperation(nullptr)
	, m_buttonRenderItem(nullptr)
	, m_currDraws(nullptr)
	, m_currButtons(nullptr)
{
}

cOperator::~cOperator()
{
}

void cOperator::SetUp()
{
	SetObjectRenderItems();

	m_operSelectButtons.SetUp({ NOMALBUTTONSIZE, NOMALBUTTONSIZE }, m_buttonRenderItem);
	m_operSelectButtons.SetPos({ 0,0,0 });

	for (int i = 0; i < OPER_MESHOPER; i++)
	{
		switch (i)
		{
		case OPER_ADD_PLANE:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Plane));
			break;
		case OPER_ADD_LINE:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Line));
			break;
		case OPER_ADD_DOT:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Dot));
			break;
		case OPER_DRAWOPER:
		case OPER_PUSH_MESH:
			m_operSelectButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperTypeSelect, this, placeholders::_1), m_operSelectButtons.GetButtonNum());
			continue;
			break;
		default:
			assert(false);
			break;
		}

		if (i < OPER_DRAWOPER)
		{
			m_drawButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), m_operations.size());
		}
		else if (i < OPER_PUSH_MESH)
		{
			m_meshButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), m_operations.size());
		}

		m_operations.back()->SetUp();
	}
}

void cOperator::Update(vector<unique_ptr<cDrawElement>>& draws, cDrawPlane* drawPlane)
{
	m_currDraws = &draws;

	if (m_currOperation)
	{
		if (!m_currOperation->GetOperState())
		{
			m_currOperation = nullptr;
		}
		else
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x0001)
			{
				m_currOperation->EndOperation();
				m_currOperation = nullptr;
			}
			else if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			{
				m_currOperation->CancleOperation(draws);
				m_currOperation = nullptr;
				
				if (m_currButtons)
				{
					m_currButtons->SetRenderState(false);
					m_currButtons = nullptr;
				}
			}
		}
	}

	if (m_currOperation == nullptr)
	{
		for (auto& it : m_operations)
		{
			if (it->GetOperState())
			{
				m_currOperation = it.get();
			}
		}

		if (m_currOperation == nullptr)
		{
			if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			{
				CAMERA.SetTarget(nullptr);
			}
		}
	}

	if (m_currOperation)
	{
		switch (m_currOperation->GetOperType())
		{
		case OPER_ADD_PLANE:
			m_currOperation->DrawElementOperation(draws, drawPlane);
			break;
		case OPER_ADD_LINE:
		case OPER_ADD_DOT:
			m_currOperation->DrawElementOperation(draws, drawPlane);
			break;
		default:
			assert(false);
			break;
		}
	}
	else
	{
		if (m_currButtons)
		{
			m_currButtons->Update();
		}
	}
}

void cOperator::SetRenderState(bool value)
{
	m_operSelectButtons.SetRenderState(value);
}

void cOperator::OperationStart(int index)
{
	assert(m_operations.size() && "It had not Setup");

	m_operations[index]->StartOperation();

	if (m_currOperation)
	{
		m_currOperation->CancleOperation(*m_currDraws);
	}

	m_currOperation = m_operations[index].get();
}

void cOperator::OperTypeSelect(int num)
{
	m_drawButtons.SetRenderState(false);
	m_meshButtons.SetRenderState(false);

	switch (num)
	{
	case 0:
		m_currButtons = &m_drawButtons;
		m_currButtons->SetRenderState(true);
		break;
	case 1:
		m_currButtons = &m_meshButtons;
		m_currButtons->SetRenderState(true);
		break;
	default:
		break;
	}
}

void cOperator::SetObjectRenderItems()
{
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

	m_buttonRenderItem = RENDERITEMMG->AddRenderItem("ui");
}
