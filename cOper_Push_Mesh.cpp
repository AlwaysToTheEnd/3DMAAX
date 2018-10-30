#include "stdafx.h"


cOper_Push_Mesh::cOper_Push_Mesh()
	: cOperation(OPER_PUSH_MESH)
	, m_draws(nullptr)
	, m_selectDrawsIndex(-1)
	, m_cycleIndex(-1)
	, m_meshHeight(0)
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

void cOper_Push_Mesh::MeshOperation(cMesh* currMesh)
{
	m_operControl.Update(XMMatrixIdentity());

	switch (m_worksSate)
	{
	case cOper_Push_Mesh::ADD_MESH_QUERY:
	{
		if (currMesh == nullptr)
		{
			EndOperation();
			return;
		}

		m_draws = nullptr;
		m_selectDrawsIndex = -1;

		vector<DrawItems*>& meshDraws = currMesh->GetDraws();
		int i = 0;
		for (auto& it : meshDraws)
		{
			m_operControl.AddParameter(L"Draws" + to_wstring(i), DXGI_FORMAT_R32_SINT, &m_selectDrawsIndex);
			i++;
		}

		if (i == 0)
		{
			EndOperation();
			return;
		}

		m_operControl.IsRenderState(true);
		m_worksSate = DRAW_SELECT;
	}
	break;
	case cOper_Push_Mesh::DRAW_SELECT:
	{
		if (m_selectDrawsIndex != -1)
		{
			m_currDrawCycleDotsList.clear();
			m_draws = currMesh->GetDraws()[m_selectDrawsIndex];
			m_currDrawCycleDotsList = currMesh->LineCycleCheck(m_selectDrawsIndex);

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

				m_operControl.IsRenderState(true);
		
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
			m_worksSate = MESH_HEIGHT_INPUT;
		}
	}
	break;
	case cOper_Push_Mesh::MESH_HEIGHT_INPUT:
	{
		XMMATRIX planeMat = XMLoadFloat4x4(&currMesh->GetDraws()[m_selectDrawsIndex]->m_plane->GetMatrix());
		XMMATRIX scaleMat = XMMatrixScaling(1, 1, m_meshHeight + 0.0001f);

		m_prevViewRenderInstance->numFramesDirty = gNumFrameResources;
		XMStoreFloat4x4(&m_prevViewRenderInstance->instanceData.World, scaleMat*planeMat);
	}
	break;
	}
}

void cOper_Push_Mesh::CancleOperation(vector<unique_ptr<cDrawElement>>& draw)
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

	vector<NT_Vertex> vertices(cycleDotsNum * 2);
	vector<UINT> indices;

	for (UINT i = 0; i < cycleDotsNum; i++)
	{
		vertices[i].pos = cycleDots[i]->GetPosC();
		vertices[i].uv = { 0,0 };
	}

	for (size_t i = cycleDotsNum; i < vertices.size(); i++)
	{
		vertices[i].pos = cycleDots[i%cycleDotsNum]->GetPosC();
		vertices[i].pos.z -= 1.0f;
		vertices[i].uv = { 0,0 };
	}

	vector<UINT> earClipingIndex(cycleDotsNum);
	iota(earClipingIndex.begin(), earClipingIndex.end(), 0);

	UINT currIndex = 0;
	while (true)
	{
		UINT firstDotsIndex = earClipingIndex[currIndex];
		UINT nextDotIndex1 = earClipingIndex[(currIndex + 1) % earClipingIndex.size()];
		UINT nextDotIndex2 = earClipingIndex[(currIndex + 2) % earClipingIndex.size()];

		XMVECTOR originVector = XMLoadFloat3(&vertices[firstDotsIndex].pos);
		XMVECTOR tryVector1 = XMLoadFloat3(&vertices[nextDotIndex1].pos) - originVector;
		XMVECTOR tryVector2 = XMLoadFloat3(&vertices[nextDotIndex2].pos) - originVector;

		XMVECTOR crossVector = XMVector3Cross(tryVector1, tryVector2);
		
		if (crossVector.m128_f32[2] > 0)
		{
			indices.push_back(firstDotsIndex);
			indices.push_back(nextDotIndex1);
			indices.push_back(nextDotIndex2);

			auto it = earClipingIndex.begin();
			for (int i = 0; i < (currIndex + 1) % earClipingIndex.size(); i++)
			{
				it++;
			}

			earClipingIndex.erase(it);
		}
		else
		{
			currIndex = (currIndex + 1) % earClipingIndex.size();
		}

		if (earClipingIndex.size() <= 3)
		{
			if (earClipingIndex.size() == 3)
			{
				originVector = XMLoadFloat3(&vertices[earClipingIndex[0]].pos);
				tryVector1 = XMLoadFloat3(&vertices[earClipingIndex[1]].pos) - originVector;
				tryVector2 = XMLoadFloat3(&vertices[earClipingIndex[2]].pos) - originVector;

				crossVector = XMVector3Cross(tryVector1, tryVector2);

				if (crossVector.m128_f32[2] < 0)
				{
					indices.push_back(earClipingIndex[0]);
					indices.push_back(earClipingIndex[2]);
					indices.push_back(earClipingIndex[1]);
				}
				else
				{
					indices.push_back(earClipingIndex[0]);
					indices.push_back(earClipingIndex[1]);
					indices.push_back(earClipingIndex[2]);
				}
			}

			break;
		}
	}

	UINT indicesNum = (UINT)indices.size();

	for (size_t i = 0; i < indicesNum; i++)
	{
		indices.push_back(indices[i] + cycleDotsNum);
	}

	UINT sideTryangleStartIndex = (UINT)vertices.size();
	vertices.insert(vertices.end(), vertices.begin(), vertices.end());

	for (UINT i = sideTryangleStartIndex; i < sideTryangleStartIndex + cycleDotsNum; i++)
	{
		indices.push_back(i);
		indices.push_back(i + cycleDotsNum);
		indices.push_back((i + 1)% cycleDotsNum + cycleDotsNum );

		indices.push_back(i);
		indices.push_back((i + 1) % cycleDotsNum + cycleDotsNum);
		indices.push_back((i + 1) % cycleDotsNum);
	}

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		XMFLOAT3 normalVector(0, 0, 0);
		XMStoreFloat3(&normalVector, 
			GetNormalFromTryangle(vertices[indices[i]].pos, vertices[indices[i+1]].pos, vertices[indices[i+2]].pos));

		vertices[indices[i]].normal = normalVector;
		vertices[indices[i+1]].normal = normalVector;
		vertices[indices[i+2]].normal = normalVector;
	}

	MESHMG->ChangeMeshGeometryData(m_previewGeo->name, vertices.data(), indices.data(),
		sizeof(NT_Vertex), sizeof(NT_Vertex)*(UINT)vertices.size(),
		DXGI_FORMAT_R32_UINT, sizeof(UINT)*(UINT)indices.size(), false);

	XMMATRIX planeMat = XMLoadFloat4x4(&currMesh->GetDraws()[m_selectDrawsIndex]->m_plane->GetMatrix());
	XMMATRIX scaleMat = XMMatrixScaling(1, 1, 0.01f);

	m_prevViewRender->SetGeometry(m_previewGeo, m_previewGeo->name);
	m_prevViewRender->SetRenderOK(true);
	m_prevViewRenderInstance->m_isRenderOK = true;
	m_prevViewRenderInstance->numFramesDirty = gNumFrameResources;
	XMStoreFloat4x4(&m_prevViewRenderInstance->instanceData.World, scaleMat*planeMat);
}

XMVECTOR XM_CALLCONV cOper_Push_Mesh::GetNormalFromTryangle(const XMFLOAT3 & pos1, const XMFLOAT3 & pos2, const XMFLOAT3 & pos3)
{
	XMVECTOR originVector = XMLoadFloat3(&pos1);
	XMVECTOR tryVector1 = XMLoadFloat3(&pos2) - originVector;
	XMVECTOR tryVector2 = XMLoadFloat3(&pos3) - originVector;
	
	return XMVector3Normalize(XMVector3Cross(tryVector1, tryVector2));
}
