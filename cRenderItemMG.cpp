#include "stdafx.h"

RenderItemMG* RenderItemMG::instance = nullptr;
ID3D12Device* cRenderItem::m_device = nullptr;

void cRenderItem::Update()
{
	if (!m_isRenderOK) return;

	if (m_numFrameDirty > 0)
	{
		SetUploadBufferSize(m_currBufferSize);
		m_numFrameDirty--;
	}

	m_currFrameCB = m_objectCB[m_currFrameCBIndex].get();

	for (auto it = m_instances.begin(); it != m_instances.end();)
	{
		if (it->use_count() == 1)
		{
			it = m_instances.erase(it);
		}
		else
		{
			it++;
		}
	}

	m_renderInstanceCount = 0;
	UINT nonRenderCount = 0;

	for (auto it = m_instances.begin(); it != m_instances.end();)
	{
		if (m_instances.size() == nonRenderCount + m_renderInstanceCount) break;

		if ((*it)->m_isRenderOK == false)
		{
			m_instances.push_back(*it);
			it = m_instances.erase(it);
			nonRenderCount++;
		}
		else
		{
			if ((*it)->numFramesDirty > 0)
			{
				InstanceData data;
				XMMATRIX world = XMLoadFloat4x4(&(*it)->instanceData.World);
				XMMATRIX texTransform = XMLoadFloat4x4(&(*it)->instanceData.TexTransform);
				data.MaterialIndex = (*it)->instanceData.MaterialIndex;
				XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
				XMStoreFloat4x4(&data.TexTransform, XMMatrixTranspose(texTransform));
				data.sizeScale = (*it)->instanceData.sizeScale;

				m_currFrameCB->CopyData(m_renderInstanceCount, data);
				(*it)->numFramesDirty = gNumFrameResources;
			}

			it++;
			m_renderInstanceCount++;
		}
	}

	m_currFrameCBIndex = (m_currFrameCBIndex + 1) % gNumFrameResources;
}

void cRenderItem::Render(ID3D12GraphicsCommandList * cmdList)
{
	if (m_isRenderOK)
	{
		cmdList->IASetVertexBuffers(0, 1, &m_geo->GetVertexBufferView());
		cmdList->IASetIndexBuffer(&m_geo->GetIndexBufferView());
		cmdList->IASetPrimitiveTopology(m_primitiveType);

		cmdList->SetGraphicsRootShaderResourceView(0,
			m_currFrameCB->Resource()->GetGPUVirtualAddress());

		cmdList->DrawIndexedInstanced(m_indexCount, m_renderInstanceCount, m_startIndexLocation,
			m_baseVertexLocation, 0);
	}
}

void cRenderItem::SetUploadBufferSize(UINT numInstance)
{
	m_objectCB[m_currFrameCBIndex] = nullptr;
	m_objectCB[m_currFrameCBIndex] = make_unique<UploadBuffer<InstanceData>>(m_device, numInstance, false);
}

void cRenderItem::SetGeometry(const MeshGeometry * geometry, string submeshName)
{
	m_geo = geometry;
	auto subMesh = m_geo->DrawArgs.find(submeshName)->second;
	m_indexCount = subMesh.indexCount;
	m_baseVertexLocation = subMesh.baseVertexLocation;
	m_startIndexLocation = subMesh.startIndexLocation;
}

shared_ptr<RenderInstance> cRenderItem::GetRenderIsntance()
{
	m_instances.push_back(make_shared<RenderInstance>());

	if (m_instances.size() > m_currBufferSize)
	{
		m_numFrameDirty = gNumFrameResources;
		m_currBufferSize = m_instances.size() * 2;
	}

	return m_instances.back();
}


void RenderItemMG::Update()
{
	for (auto& it : m_RenderItemSets)
	{
		for (auto& it2 : it.second.items)
		{
			it2->Update();
		}
	}

	//Concurrency::parallel_for_each(m_RenderItemSets.begin(), m_RenderItemSets.end(),
	//[](std::pair<const string, RenderItemSet> renderItemset)
	//{
	//	for (auto it = renderItemset.second.items.begin();
	//		it != renderItemset.second.items.end();)
	//	{
	//		if (it->use_count() == 1)
	//		{
	//			it = renderItemset.second.items.erase(it);
	//		}
	//		else
	//		{
	//			(*it)->Update();
	//			it++;
	//		}
	//	}
	//});
}

void RenderItemMG::Render(ID3D12GraphicsCommandList * cmdList, string renderItemSetName)
{
	auto it = m_RenderItemSets.find(renderItemSetName);

	assert(it != m_RenderItemSets.end());

	for (auto& it2 : it->second.items)
	{
		if (it2->m_instances.size())
		{
			it2->Render(cmdList);
		}
	}
}

void RenderItemMG::AddRenderSet(string renderSetKeyName)
{
	auto iter = m_RenderItemSets.find(renderSetKeyName);

	assert(iter == m_RenderItemSets.end());

	if (m_RenderItemSets.size() == 0)
	{
		baseKey = renderSetKeyName;
	}

	m_RenderItemSets[renderSetKeyName];
}

RenderItemSet * RenderItemMG::GetRenderItemSet(string key)
{
	auto iter = m_RenderItemSets.find(key);
	assert(iter != m_RenderItemSets.end());

	return &iter->second;
}

shared_ptr<cRenderItem> RenderItemMG::AddRenderItem(string renderSetKeyName)
{
	if (renderSetKeyName == "")
	{
		renderSetKeyName = baseKey;
	}

	auto iter = m_RenderItemSets.find(renderSetKeyName);
	assert(iter != m_RenderItemSets.end());

	iter->second.items.push_back(make_shared<cRenderItem>());

	return iter->second.items.back();
}


