#include "stdafx.h"


cOper_Push_Mesh::cOper_Push_Mesh()
	: cOperation(OPER_PUSH_MESH)
	, m_draws(nullptr)
	, m_cycleIndex(-1)
	, m_meshHeight(0)
	, m_isCreateMesh(false)
	, m_workType(CSG_UNION)
{
}


cOper_Push_Mesh::~cOper_Push_Mesh()
{

}

UINT cOper_Push_Mesh::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	switch (m_worksSate)
	{
	case cOper_Push_Mesh::ADD_MESH_QUERY:
	{
		if (CurrMeshCheckAndPick(meshs, currMesh))
		{
			m_isCreateMesh = false;
			m_draws = nullptr;
			m_operationText->printString = L"그림 오브젝트 선택";
			m_operationText->isRender = true;
			m_worksSate = DRAW_SELECT;
		}
	}
	break;
	case cOper_Push_Mesh::DRAW_SELECT:
	{
		if (CurrDrawCheckAndPick(drawItems, currDrawItems))
		{
			m_currDrawCycleDotsList.clear();

			m_draws = currDrawItems;
			m_draws->SetPickRender(2);
			m_draws->SetRenderState(true);
			m_currDrawCycleDotsList = LineCycleCheck(m_draws);
			CAMERA.SetTargetAndSettingAngle(m_draws->m_plane);

			if (!m_currDrawCycleDotsList.empty())
			{
				int i = 0;
				m_cycleIndex = -1;
				for (auto& it : m_currDrawCycleDotsList)
				{
					m_operControl->AddParameter(L"Cycle" + to_wstring(i) + L"(" + to_wstring(it.size()) + L")",
						OPERDATATYPE_INDEX, &m_cycleIndex);
					i++;
				}

				m_operationText->printString = L"Select Cycle";
				UIMG->UIOn(m_operControl);
				m_worksSate = CYCLE_SELECT;
			}
		}
	}
	break;
	case cOper_Push_Mesh::CYCLE_SELECT:
	{
		if (m_cycleIndex != -1)
		{
			PreviewPushMeshCreate(currMesh);
			m_operControl->ClearParameters();
			m_operControl->AddParameter(L"Hegith value : ", OPERDATATYPE_FLOAT, &m_meshHeight);
			m_operControl->AddParameter(L"UINION", OPERDATATYPE_INDEX, &m_workType);
			m_operControl->AddParameter(L"DIFFERENCE", OPERDATATYPE_INDEX, &m_workType);
			m_operControl->AddParameter(L"INTERSECTION", OPERDATATYPE_INDEX, &m_workType);
			m_operControl->AddParameter(L"Create Mesh", OPERDATATYPE_BOOL, &m_isCreateMesh);
			m_worksSate = MESH_ATTRIBUTE_INPUT;
		}
	}
	break;
	case cOper_Push_Mesh::MESH_ATTRIBUTE_INPUT:
	{
		XMMATRIX planeMat = XMLoadFloat4x4(&m_draws->m_plane->GetMatrix());
		XMMATRIX scaleMat;
		XMMATRIX translationMat = XMMatrixIdentity();

		switch (m_workType)
		{
		case CSG_UNION:
			m_operationText->printString = L"UNION";
			break;
		case CSG_DIFFERENCE:
			m_operationText->printString = L"DIFFERENCE";
			break;
		case CSG_INTERSECTION:
			m_operationText->printString = L"INTERSECTION";
			break;
		}

		if (m_meshHeight == 0)
		{
			scaleMat = XMMatrixScaling(1, 1, 0.001f);
		}
		else if (m_meshHeight < 0)
		{
			translationMat = XMMatrixTranslation(0, 0, -m_meshHeight);
			scaleMat = XMMatrixScaling(1, 1, -m_meshHeight);
		}
		else
		{
			scaleMat = XMMatrixScaling(1, 1, m_meshHeight);
		}

		if (m_isCreateMesh)
		{
			if (m_meshHeight == 0) return 0;

			unique_ptr<cCSGObject> scgObject(new cCSGObject);
			scgObject->SetData(m_vertices, m_indices, scaleMat*translationMat*planeMat);
			currMesh->AddCSGObject((CSGWORKTYPE)(m_workType), move(scgObject));
			currMesh->SubObjectSubAbsorption();
			currMesh->SetDrawItems(m_draws);
			m_draws->SetRenderState(false);
			currDrawItems = nullptr;
			EndOperation();
		}
		else
		{
			m_prevViewRenderInstance->numFramesDirty = gNumFrameResources;
			XMStoreFloat4x4(&m_prevViewRenderInstance->instanceData.World, scaleMat*translationMat*planeMat);
		}
	}
	break;
	}

	return 0;
}

void cOper_Push_Mesh::CancleOperation(DrawItems* draw)
{
	EndOperation();
}

void cOper_Push_Mesh::PreviewPushMeshCreate(cMesh* currMesh)
{
	auto iter = m_currDrawCycleDotsList.begin();
	for (int i = 0; i < m_cycleIndex; i++) iter++;
	auto& cycleDots = *iter;

	UINT cycleDotsNum = (UINT)cycleDots.size();
	assert(cycleDotsNum > 2);

	if (CheckCWCycle(cycleDots))
	{
		vector<const cDot*> temp;

		for (int i = (int)cycleDots.size() - 1; i >= 0; i--)
		{
			temp.push_back(cycleDots[i]);
		}

		cycleDots.assign(temp.begin(), temp.end());
	}

	m_vertices.clear();
	m_indices.clear();

	XMFLOAT4 edgeRect(FLT_MAX, -FLT_MAX, -FLT_MAX, FLT_MAX);

	for (UINT i = 0; i < cycleDotsNum; i++)
	{
		m_vertices.push_back({});
		m_vertices.back().pos = cycleDots[i]->GetPosC();
		m_vertices.back().uv = { 0,0 };

		if (m_vertices.back().pos.x < edgeRect.x)
		{
			edgeRect.x = m_vertices.back().pos.x;
		}

		if (m_vertices.back().pos.x > edgeRect.z)
		{
			edgeRect.z = m_vertices.back().pos.x;
		}

		if (m_vertices.back().pos.y > edgeRect.y)
		{
			edgeRect.y = m_vertices.back().pos.y;
		}

		if (m_vertices.back().pos.y < edgeRect.w)
		{
			edgeRect.w = m_vertices.back().pos.y;
		}
	}

	float edgeRectSizeX = edgeRect.z - edgeRect.x;
	float edgeRectSizeY = edgeRect.y - edgeRect.w;

	for (UINT i = 0; i < cycleDotsNum; i++)
	{
		m_vertices[i].uv.x = (m_vertices[i].pos.x - edgeRect.x) / edgeRectSizeX;
		m_vertices[i].uv.y = (m_vertices[i].pos.y - edgeRect.w) / edgeRectSizeY;
	}

	EarClipping<NT_Vertex, UINT>(m_vertices, m_indices);
	m_vertices.insert(m_vertices.end(), m_vertices.begin(), m_vertices.end());

	for (UINT i = cycleDotsNum; i < cycleDotsNum * 2; i++)
	{
		m_vertices[i].pos.z -= 1.0f;
	}

	UINT indicesNum = (UINT)m_indices.size();

	for (size_t i = 0; i < indicesNum; i += 3)
	{
		m_indices.push_back(m_indices[i] + cycleDotsNum);
		m_indices.push_back(m_indices[i + 2] + cycleDotsNum);
		m_indices.push_back(m_indices[i + 1] + cycleDotsNum);
	}

	vector<float> weightLength(cycleDotsNum+1);
	float cycleLength = 0;

	weightLength.front() = 0.0f;
	for (UINT i = 0; i < cycleDotsNum-1; i++)
	{
		float lineDist= XMVectorGetX(XMVector3Length(
			XMLoadFloat3(&m_vertices[i + 1].pos) - XMLoadFloat3(&m_vertices[i].pos)));
		cycleLength += lineDist;
		weightLength[i+1] = cycleLength;
	}
	weightLength.back() = 1.0f;

	for (UINT i = 0; i < cycleDotsNum; i++)
	{
		UINT currIndex = (UINT)m_vertices.size();
		float startUV_X = weightLength[i] / cycleLength;
		float endUV_X = weightLength[i+1] / cycleLength;
		m_vertices.push_back(m_vertices[i]);
		m_vertices.push_back(m_vertices[i + cycleDotsNum]);
		m_vertices.push_back(m_vertices[(i + 1) % cycleDotsNum + cycleDotsNum]);
		m_vertices.push_back(m_vertices[(i + 1) % cycleDotsNum]);
		m_vertices[currIndex].uv.y = 0.0f;
		m_vertices[currIndex].uv.x = startUV_X;
		m_vertices[currIndex + 1].uv.y = 1.0f;
		m_vertices[currIndex + 1].uv.x = startUV_X;
		m_vertices[currIndex + 2].uv.y = 1.0f;
		m_vertices[currIndex + 2].uv.x = endUV_X;
		m_vertices[currIndex + 3].uv.y = 0.0f;
		m_vertices[currIndex + 3].uv.x = endUV_X;

		m_indices.push_back(currIndex);
		m_indices.push_back(currIndex + 1);
		m_indices.push_back(currIndex + 2);

		m_indices.push_back(currIndex);
		m_indices.push_back(currIndex + 2);
		m_indices.push_back(currIndex + 3);
	}

	for (size_t i = 0; i < m_indices.size(); i += 3)
	{
		XMFLOAT3 normalVector(0, 0, 0);
		XMStoreFloat3(&normalVector, XMVector3Normalize(
			GetNormalFromTriangle(m_vertices[m_indices[i]].pos, m_vertices[m_indices[i + 1]].pos, m_vertices[m_indices[i + 2]].pos)));

		m_vertices[m_indices[i]].normal = normalVector;
		m_vertices[m_indices[i + 1]].normal = normalVector;
		m_vertices[m_indices[i + 2]].normal = normalVector;
	}

	MESHMG->ChangeMeshGeometryData(m_previewGeo->name, m_vertices.data(), m_indices.data(),
		sizeof(NT_Vertex), sizeof(NT_Vertex)*(UINT)m_vertices.size(),
		DXGI_FORMAT_R32_UINT, sizeof(UINT)*(UINT)m_indices.size(), false);

	XMMATRIX planeMat = XMLoadFloat4x4(&m_draws->m_plane->GetMatrix());

	m_prevViewRender->SetGeometry(m_previewGeo, m_previewGeo->name);
	m_prevViewRender->SetRenderOK(true);
	m_prevViewRenderInstance->m_isRenderOK = true;
	m_prevViewRenderInstance->numFramesDirty = gNumFrameResources;
	m_prevViewRenderInstance->instanceData.MaterialIndex = 7;
	XMStoreFloat4x4(&m_prevViewRenderInstance->instanceData.World, planeMat);
}