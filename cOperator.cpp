#include "stdafx.h"


cOperator::cOperator()
	: m_currOperation(nullptr)
	, m_buttonRenderItem(nullptr)
{
}


cOperator::~cOperator()
{
}

void cOperator::SetUp(shared_ptr<cRenderItem> buttonRenderItem)
{
	m_buttonRenderItem = buttonRenderItem;

	m_drawButtonCollector.SetUp({ NOMALBUTTONSIZE, NOMALBUTTONSIZE }, m_buttonRenderItem);
	m_drawButtonCollector.SetPos({ 0,0,0 });
	m_meshButtonCollector.SetUp({ NOMALBUTTONSIZE, NOMALBUTTONSIZE }, m_buttonRenderItem);
	m_meshButtonCollector.SetPos({ 0,NOMALBUTTONSIZE,0 });

	for (int i = 0; i < OPER_MESHOPER_COUNT; i++)
	{
		switch (i)
		{
		case OPER_ADD_PLANE:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Plane));
			break;
		case OPER_ADD_LINE:
			m_operations.push_back(unique_ptr<cOperation>(new cOper_Add_Plane));
			break;
		case OPER_PUSH_MESH:
			continue;
			break;
		case OPER_DRAWOPER_COUNT:
			continue;
			break;
		default:
			assert(false);
			break;
		}

		m_operations.back()->SetUp();
		m_drawButtonCollector.AddButton(m_ButtonMtlTexBaseIndex + i,
			bind(&cOperator::OperationStart, this, placeholders::_1), i);
	}
}

void cOperator::Update(vector<unique_ptr<cDrawElement>>& planes)
{
	if (m_currOperation)
	{
		if (!m_currOperation->GetOperState())
		{
			m_currOperation = nullptr;
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
	}

	if (m_currOperation)
	{
		switch (m_currOperation->GetOperType())
		{
		case OPER_ADD_PLANE:
			m_currOperation->DrawElementOperation(planes);
			break;
		case OPER_ADD_LINE:
			m_currOperation->DrawElementOperation(planes);
			break;
		default:
			break;
		}
	}
	else
	{
		m_drawButtonCollector.Update();
		m_meshButtonCollector.Update();
	}
}

void cOperator::OperationStart(int type)
{
	assert(m_operations.size() && "It had not Setup");

	switch (type)
	{
	case OPER_ADD_PLANE:
	case OPER_ADD_LINE:
		m_operations[type]->StartOperation();

		if (m_currOperation)
		{
			m_currOperation->CancleOperation();
		}

		m_currOperation = m_operations[type].get();
		break;
	case OPER_PUSH_MESH:
		m_operations[type-1]->StartOperation();
		m_currOperation = m_operations[type-1].get();
		break;
	default:
		assert(false);
		break;
	}
}
