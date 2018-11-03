#include "stdafx.h"

cOperator::cOperator()
	: m_currOperation(nullptr)
	, m_buttonRenderItem(nullptr)
	, m_currDraws(nullptr)
	, m_currMesh(nullptr)
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

	for (int i = 0; i < OPER_COUNT; i++)
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
		case OPER_PUSH_MESH:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Push_Mesh));
			break;
		case OPER_DRAWOPER:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Draws));
			break;
		case OPER_MESHOPER:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Mesh));
			break;
		default:
			assert(false);
			break;
		}

		m_operSelectButtons.AddButton(m_ButtonMtlTexBaseIndex + i,
			bind(&cOperator::OperationStart, this, placeholders::_1), i);

		m_operations.back()->SetUp();
	}
}

void cOperator::Update()
{
	if (OperationCheck())
	{
		m_currOperation->OperationUpdate(m_drawItems, m_planes, m_mesh, m_currDraws, m_currMesh);
	}

	m_operSelectButtons.Update();
	m_planes.Update();
}

void cOperator::SetRenderState(bool value)
{
	m_operSelectButtons.SetRenderState(value);
}

bool cOperator::OperationCheck()
{
	if (m_currOperation)
	{
		if (!m_currOperation->GetOperState())
		{
			if (m_currDraws)
			{
				m_currDraws->SetPickRender(2);
			}

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
				break;
			}
		}

		if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
		{
			m_currDraws = nullptr;
			CAMERA.SetTarget(nullptr);
		}
	}

	return m_currOperation != nullptr;
}

void cOperator::OperationStart(int index)
{
	assert(m_operations.size() && "It had not Setup");

	if (index == OPER_DRAWOPER)
	{
		if (m_currDraws)
		{
			m_currDraws->SetPickRender(0);
		}
	}

	m_operations[index]->StartOperation();

	if (m_currOperation)
	{
		m_currOperation->CancleOperation(m_currDraws->m_draws);
	}

	m_currOperation = m_operations[index].get();
}
