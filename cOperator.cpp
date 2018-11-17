#include "stdafx.h"

cOperator::cOperator()
	: m_currOperation(nullptr)
	, m_currDraws(nullptr)
	, m_currMesh(nullptr)
	, m_objectListUI(nullptr)
	, m_objectControl(nullptr)
{
}

cOperator::~cOperator()
{
}

void cOperator::Build()
{
	m_planes.SetRenderItem(RENDERITEMMG->AddRenderItem("plane"));
	m_objectListUI = UIMG->AddUI<cUIOperWindow>("objectListUI");
	m_objectControl= UIMG->AddUI<cUIOperWindow>("objectControlUI");
	m_objectListUI->SetPos({ 790,50,0 });
	UIMG->UIOn(m_objectListUI);
	m_objectControl->SetPos({ 790,250,0 });
	UIMG->UIOn(m_objectControl);

	m_operSelectButtons.Build({ NOMALBUTTONSIZE, NOMALBUTTONSIZE });
	m_operSelectButtons.SetPos({ 0,3,0 });

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

		m_operSelectButtons.AddButton("operatorButton"+to_string(i) ,m_ButtonMtlTexBaseIndex + i,
			bind(&cOperator::OperationStart, this, placeholders::_1), i);

		m_operations.back()->Build();
	}

	m_operSelectButtons.SetRenderState(true);
	ObjectListUIUpdate();
}

void cOperator::Update()
{
	if (OperationCheck())
	{
		m_currOperation->OperationUpdate(m_drawItems, m_planes, m_mesh, m_currDraws, m_currMesh);
	}

	if (m_currDraws)
	{
		for (auto& it : m_currDraws->m_draws)
		{
			it->Update();
		}
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
			m_currOperation = nullptr;
			ObjectListUIUpdate();
		}
		else
		{
			if (GetAsyncKeyState(VK_SPACE) & 0x0001)
			{
				m_currOperation->EndOperation();
				ObjectListUIUpdate();
				m_currOperation = nullptr;
			}
			else if (GetAsyncKeyState(VK_ESCAPE) & 0x0001)
			{
				m_currOperation->CancleOperation(m_currDraws);
				ObjectListUIUpdate();
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
			OBJCOORD->ObjectRegistration(nullptr);
			UIMG->UIOff(m_objectControl);

			if (m_currDraws)
			{
				m_currDraws->SetPickRender(0);
			}

			m_currDraws = nullptr;
			CAMERA.SetTargetAndSettingAngle(nullptr);
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
		m_currOperation->CancleOperation(m_currDraws);
	}

	m_currOperation = m_operations[index].get();
}

void cOperator::ObjectListUIUpdate()
{
	m_objectListUI->ClearParameters();

	auto planes = m_planes.GetObjectsPtr<cPlane>();

	UINT i = 0;
	m_objectListUI->AddParameter(L"PLANE LIST", OPERDATATYPE_NONE, (void*)nullptr);
	for (auto it:planes)
	{
		m_objectListUI->AddParameter(L"  plane "+to_wstring(i),
			bind(&cOperator::SelectPlane, this, placeholders::_1), (UINT64)it);
		i++;
	}

	m_objectListUI->AddParameter(L"DRAW LIST", OPERDATATYPE_NONE, (void*)nullptr);
	for (auto& it : m_drawItems)
	{
		m_objectListUI->AddParameter(L"  "+it.first, 
			bind(&cOperator::SelectDraws, this, placeholders::_1), (UINT64)&it.second);
	}

	m_objectListUI->AddParameter(L"MESH INFO", OPERDATATYPE_NONE, (void*)nullptr);
	for (auto& it : m_mesh)
	{
		m_objectListUI->AddParameter(L"  "+it.first, 
			bind(&cOperator::SelectMeshs, this, placeholders::_1), (UINT64)&it.second);

		if (&it.second == m_currMesh)
		{
			cCSGObject* rootCSG = it.second.GetCSGRootObject();
			auto& childSCGObjects = rootCSG->GetChilds();

			for (size_t i = 0; i < childSCGObjects.size(); i++)
			{
				wstring csgName = L"push "+to_wstring(i);

				switch (childSCGObjects[i]->GetType())
				{
				case CSG_UNION:
					csgName += L"(Union)";
					break;
				case CSG_DIFFERENCE:
					csgName += L"(Dif)";
					break;
				case CSG_INTERSECTION:
					csgName += L"(Inter)";
					break;
				default:
					break;
				}

				m_objectListUI->AddParameter(L"    " + csgName,
					bind(&cOperator::SelectCSGObjectOn, this, placeholders::_1), (UINT64)childSCGObjects[i].get());
			}
		}
	}
}

void cOperator::SelectPlane(UINT64 address)
{
	m_objectControl->ClearParameters();
	UIMG->UIOn(m_objectControl);

	cPlane* plane = reinterpret_cast<cPlane*>(address);

	if (!m_currOperation)
	{
		OBJCOORD->ObjectRegistration(plane);
	}
	
	m_objectControl->AddParameter(L"quater X : ", OPERDATATYPE_FLOAT, (void*)&plane->GetQuaternion().x);
	m_objectControl->AddParameter(L"quater Y : ", OPERDATATYPE_FLOAT, (void*)&plane->GetQuaternion().y);
	m_objectControl->AddParameter(L"quater Z : ", OPERDATATYPE_FLOAT, (void*)&plane->GetQuaternion().z);
	m_objectControl->AddParameter(L"quater W : ", OPERDATATYPE_FLOAT, (void*)&plane->GetQuaternion().w);
	m_objectControl->AddParameter(L"pos    X : ", OPERDATATYPE_FLOAT, (void*)&plane->GetPos().x);
	m_objectControl->AddParameter(L"pos    Y : ", OPERDATATYPE_FLOAT, (void*)&plane->GetPos().y);
	m_objectControl->AddParameter(L"pos    Z : ", OPERDATATYPE_FLOAT, (void*)&plane->GetPos().z);
}

void cOperator::SelectDraws(UINT64 address)
{
	m_objectControl->ClearParameters();

	DrawItems* draws = reinterpret_cast<DrawItems*>(address);

	if (m_currDraws == nullptr)
	{
		m_currDraws = draws;
	}
}

void cOperator::SelectMeshs(UINT64 address)
{
	m_objectControl->ClearParameters();


}

void cOperator::SelectCSGObjectOn(UINT64 address)
{
	m_objectControl->ClearParameters();

	cCSGObject* scgObject= reinterpret_cast<cCSGObject*>(address);
	scgObject->SetOnState(!scgObject->GetOnState());

	if (m_currMesh)
	{
		m_currMesh->SubObjectSubAbsorption();
	}
}
