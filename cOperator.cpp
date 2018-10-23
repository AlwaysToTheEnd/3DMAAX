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
	m_buttonRenderItem = RENDERITEMMG->AddRenderItem("ui");
	m_planes.SetRenderItem(RENDERITEMMG->AddRenderItem("plane"));

	m_operSelectButtons.SetUp({ NOMALBUTTONSIZE, NOMALBUTTONSIZE }, m_buttonRenderItem);
	m_operSelectButtons.SetPos({ 0,0,0 });

	m_drawButtons.SetUp({ NOMALBUTTONSIZE, NOMALBUTTONSIZE }, m_buttonRenderItem);
	m_drawButtons.SetPos({ 0,NOMALBUTTONSIZE,0 });
	m_drawButtons.SetRenderState(false);

	m_meshButtons.SetUp({ NOMALBUTTONSIZE, NOMALBUTTONSIZE }, m_buttonRenderItem);
	m_meshButtons.SetPos({ 0,NOMALBUTTONSIZE,0 });
	m_meshButtons.SetRenderState(false);

	for (int i = 0; i < OPER_MESHOPER; i++)
	{
		switch (i)
		{
		case OPER_ADD_PLANE:
			m_operSelectButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), i);
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Plane));
			m_operations.back()->SetUp();
			continue;
		case OPER_ADD_LINE:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Line));
			break;
		case OPER_ADD_DOT:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Dot));
			break;
		case OPER_DRAWOPER:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Draws));
			m_operSelectButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), i);
			m_operations.back()->SetUp();
			break;
		case OPER_PUSH_MESH:
			m_operSelectButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), i);
			continue;
		default:
			assert(false);
			break;
		}

		if (i < OPER_DRAWOPER)
		{
			m_drawButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), i);
		}
		else if (i < OPER_PUSH_MESH)
		{
			m_meshButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
				bind(&cOperator::OperationStart, this, placeholders::_1), i);
		}

		m_operations.back()->SetUp();
	}
}

void cOperator::Update()
{
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
				m_currOperation->CancleOperation(m_currDraws->m_draws);
				m_currOperation = nullptr;
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
				if (m_currButtons)
				{
					m_currButtons->SetRenderState(false);
					m_currButtons = nullptr;
				}

				m_currDraws->SetPickRender(0);

				for (auto& it : m_currDraws->m_draws)
				{
					it->Update();
				}

				m_currDraws = nullptr;
				CAMERA.SetTarget(nullptr);
			}
		}
	}

	if (m_currOperation)
	{
		switch (m_currOperation->GetOperType())
		{
		case OPER_ADD_PLANE:
			m_currOperation->PlaneAddOperation(m_planes);
			break;
		case OPER_ADD_LINE:
		case OPER_ADD_DOT:
			m_currOperation->DrawElementOperation(m_currDraws);
			break;
		case OPER_DRAWOPER:
			m_currDraws = m_currOperation->DrawsAddOperatioin(m_drawItems, m_planes);
			if (m_currDraws)
			{
				m_currDraws->SetPickRender(2);
				CAMERA.SetTarget(m_currDraws->m_plane);
			}
			break;
		default:
			assert(false);
			break;
		}
	}

	if (m_currDraws)
	{
		for (auto& it : m_currDraws->m_draws)
		{
			it->Update();
		}
	}

	m_operSelectButtons.Update();

	if (m_currButtons)
	{
		m_currButtons->Update();
	}

	m_planes.Update();
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
		m_currOperation->CancleOperation(m_currDraws->m_draws);
	}

	if (index == OPER_DRAWOPER)
	{
		OperTypeSelect(0);
	}
	else if (index == OPER_DRAWOPER)
	{
		OperTypeSelect(1);
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
