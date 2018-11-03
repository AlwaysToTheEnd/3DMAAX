#include "stdafx.h"


cOper_Push_Mesh::cOper_Push_Mesh()
	: cOperation(OPER_PUSH_MESH)
	, m_draws(nullptr)
	, m_selectDrawsIndex(-1)
	, m_cycleIndex(-1)
	, m_meshHeight(0)
	, m_isCreateMesh(false)
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

UINT cOper_Push_Mesh::OperationUpdate(unordered_map<wstring, DrawItems>& drawItems,
	cDrawPlane& planes, unordered_map<wstring, cMesh>& meshs, DrawItems*& currDrawItems, cMesh*& currMesh)
{
	m_operControl.Update(XMMatrixIdentity());

	switch (m_worksSate)
	{
	case cOper_Push_Mesh::ADD_MESH_QUERY:
	{
		if (CurrMeshCheckAndPick(meshs, currMesh))
		{
			m_isCreateMesh = false;
			m_draws = nullptr;
			m_selectDrawsIndex = -1;

			int i = 0;
			for (auto& it : drawItems)
			{
				it.second.SetRenderState(true);
				m_operControl.AddParameter(it.first, DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
				i++;
			}

			if (i == 0)
			{
				EndOperation();
				return 0;
			}

			m_operControl.SetRenderState(true);
			m_worksSate = DRAW_SELECT;
		}
	}
	break;
	case cOper_Push_Mesh::DRAW_SELECT:
	{
		if (m_selectDrawsIndex != -1)
		{
			m_currDrawCycleDotsList.clear();
			auto it = drawItems.begin();
			for (int i = 0; i < m_selectDrawsIndex; i++)
			{
				it++;
			}
			m_draws = &it->second;
			m_currDrawCycleDotsList = LineCycleCheck(m_draws);
			CAMERA.SetTargetAndSettingAngle(m_draws->m_plane);

			if (m_currDrawCycleDotsList.empty())
			{
				m_selectDrawsIndex = -1;
			}
			else
			{
				int i = 0;
				m_cycleIndex = -1;
				m_operControl.ClearParameters();
				for (auto& it : m_currDrawCycleDotsList)
				{
					m_operControl.AddParameter(L"Cycle" + to_wstring(i) + L"(" + to_wstring(it.size()) + L")",
						DXGI_FORMAT_R32_SINT, &m_cycleIndex);
					i++;
				}

				for (auto& it : drawItems)
				{
					if (&it.second != m_draws)
					{
						it.second.SetRenderState(false);
					}

				}

				m_operControl.SetRenderState(true);
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
			m_operControl.ClearParameters();
			m_operControl.AddParameter(L"Hegith value : ", DXGI_FORMAT_R32_FLOAT, &m_meshHeight);
			m_operControl.AddParameter(L"Create Mesh", DXGI_FORMAT_UNKNOWN, &m_isCreateMesh);
			m_worksSate = MESH_HEIGHT_INPUT;
		}
	}
	break;
	case cOper_Push_Mesh::MESH_HEIGHT_INPUT:
	{
		XMMATRIX planeMat = XMLoadFloat4x4(&m_draws->m_plane->GetMatrix());
		XMMATRIX scaleMat;
		XMMATRIX translationMat = XMMatrixIdentity();

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
			currMesh->AddCSGObject(CSG_UNION, move(scgObject));
			currMesh->SubObjectSubAbsorption();
			m_draws->SetRenderState(false);
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

	for (UINT i = 0; i < cycleDotsNum; i++)
	{
		m_vertices.push_back({});
		m_vertices.back().pos = cycleDots[i]->GetPosC();
		m_vertices.back().uv = { 0,0 };
	}

	vector<UINT> earClipingIndex(cycleDotsNum);
	iota(earClipingIndex.begin(), earClipingIndex.end(), 0);

	UINT currIndex = 0;
	while (true)
	{
		UINT firstDotsIndex = earClipingIndex[currIndex];
		UINT nextDotIndex1 = earClipingIndex[(currIndex + 1) % earClipingIndex.size()];
		UINT nextDotIndex2 = earClipingIndex[(currIndex + 2) % earClipingIndex.size()];

		XMVECTOR originVector = XMLoadFloat3(&m_vertices[firstDotsIndex].pos);
		XMVECTOR tryVector1 = XMLoadFloat3(&m_vertices[nextDotIndex1].pos) - originVector;
		XMVECTOR tryVector2 = XMLoadFloat3(&m_vertices[nextDotIndex2].pos) - originVector;

		XMVECTOR crossVector = XMVector3Cross(tryVector1, tryVector2);

		if (crossVector.m128_f32[2] > 0)
		{
			//dot in triangle check

			bool isInsertDot = false;
			float tryVector1Length = XMVector3Length(tryVector1).m128_f32[0];
			float tryVector2Length = XMVector3Length(tryVector2).m128_f32[0];
			XMVECTOR normaTryVector1 = XMVector3Normalize(tryVector1);
			XMVECTOR normaTryVector2 = XMVector3Normalize(tryVector2);

			for (size_t i = 0; i < m_vertices.size(); i++)
			{
				if (i != firstDotsIndex && i != nextDotIndex1 && i != nextDotIndex2)
				{
					XMVECTOR insertCheckVector = XMLoadFloat3(&m_vertices[i].pos) - originVector;
					float try1dot;
					float try2dot;
					XMStoreFloat(&try1dot, XMVector3Dot(insertCheckVector, normaTryVector1));
					XMStoreFloat(&try2dot, XMVector3Dot(insertCheckVector, normaTryVector2));
					float u = try1dot / tryVector1Length;
					float v = try2dot / tryVector2Length;

					if (u >= 0 && v >= 0 && u <= 1.0f && v <= 1.0f)
					{
						if (XMVector3Cross(tryVector1, insertCheckVector).m128_f32[2] > 0 &&
							XMVector3Cross(tryVector2, insertCheckVector).m128_f32[2] < 0)
						{
							isInsertDot = true;
							break;
						}
					}
				}
			}

			if (!isInsertDot)
			{
				m_indices.push_back(firstDotsIndex);
				m_indices.push_back(nextDotIndex1);
				m_indices.push_back(nextDotIndex2);

				auto it = earClipingIndex.begin();
				for (int i = 0; i < (currIndex + 1) % earClipingIndex.size(); i++)
				{
					it++;
				}

				earClipingIndex.erase(it);

				if (earClipingIndex.size() == currIndex)
				{
					currIndex--;
				}
			}
			else
			{
				currIndex = (currIndex + 1) % earClipingIndex.size();
			}
		}
		else
		{
			currIndex = (currIndex + 1) % earClipingIndex.size();
		}

		if (earClipingIndex.size() <= 3)
		{
			if (earClipingIndex.size() == 3)
			{
				originVector = XMLoadFloat3(&m_vertices[earClipingIndex[0]].pos);
				tryVector1 = XMLoadFloat3(&m_vertices[earClipingIndex[1]].pos) - originVector;
				tryVector2 = XMLoadFloat3(&m_vertices[earClipingIndex[2]].pos) - originVector;

				crossVector = XMVector3Cross(tryVector1, tryVector2);

				if (crossVector.m128_f32[2] < 0)
				{
					m_indices.push_back(earClipingIndex[0]);
					m_indices.push_back(earClipingIndex[2]);
					m_indices.push_back(earClipingIndex[1]);
				}
				else
				{
					m_indices.push_back(earClipingIndex[0]);
					m_indices.push_back(earClipingIndex[1]);
					m_indices.push_back(earClipingIndex[2]);
				}
			}

			break;
		}
	}

	for (UINT i = 0; i < cycleDotsNum; i++)
	{
		NT_Vertex vertex;
		vertex.pos = cycleDots[i]->GetPosC();
		vertex.pos.z -= 1.0f;
		vertex.uv = { 0,0 };

		m_vertices.push_back(vertex);
	}

	UINT indicesNum = (UINT)m_indices.size();

	for (size_t i = 0; i < indicesNum; i += 3)
	{
		m_indices.push_back(m_indices[i] + cycleDotsNum);
		m_indices.push_back(m_indices[i + 2] + cycleDotsNum);
		m_indices.push_back(m_indices[i + 1] + cycleDotsNum);
	}

	UINT sideTriangleStartIndex = (UINT)m_vertices.size();
	m_vertices.insert(m_vertices.end(), m_vertices.begin(), m_vertices.end());

	for (UINT i = sideTriangleStartIndex; i < sideTriangleStartIndex + cycleDotsNum; i++)
	{
		m_indices.push_back(i);
		m_indices.push_back(i + cycleDotsNum);
		m_indices.push_back((i + 1) % cycleDotsNum + cycleDotsNum);

		m_indices.push_back(i);
		m_indices.push_back((i + 1) % cycleDotsNum + cycleDotsNum);
		m_indices.push_back((i + 1) % cycleDotsNum);
	}

	for (size_t i = 0; i < m_indices.size(); i += 3)
	{
		XMFLOAT3 normalVector(0, 0, 0);
		XMStoreFloat3(&normalVector,
			GetNormalFromTriangle(m_vertices[m_indices[i]].pos, m_vertices[m_indices[i + 1]].pos, m_vertices[m_indices[i + 2]].pos));

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
	XMStoreFloat4x4(&m_prevViewRenderInstance->instanceData.World, planeMat);
}