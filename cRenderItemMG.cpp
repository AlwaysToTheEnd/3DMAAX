#include "stdafx.h"

cRenderItemMG* cRenderItemMG::instance = nullptr;

void cRenderItemMG::Update(FrameResource* currFrameResource)
{
	auto currObjectCB = currFrameResource->objectCB.get();

	for (auto& mapIter : m_RenderItemSets)
	{
		for (auto& it = mapIter.second.items.begin(); it != mapIter.second.items.end();)
		{
			if (it->use_count() <= 1)
			{
				it = mapIter.second.items.erase(it);
			}
			else
			{
				if ((*it)->numFramesDirty > 0)
				{
					XMMATRIX world = XMLoadFloat4x4(&(*it)->world);
					XMMATRIX texTransform = XMLoadFloat4x4(&(*it)->texTransform);

					ObjectConstants objConstants;
					XMStoreFloat4x4(&objConstants.world, XMMatrixTranspose(world));
					XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

					currObjectCB->CopyData((*it)->objCBIndex, objConstants);

					(*it)->numFramesDirty--;
				}

				it++;
			}
		}
	}
}

void cRenderItemMG::Render(ID3D12GraphicsCommandList * cmdList,
	FrameResource* currFrameResource, string renderItemSetName)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto frameObjCB = currFrameResource->objectCB->Resource();
	auto frameMaterCB = currFrameResource->materialCB->Resource();

	vector<shared_ptr<RenderItem>>* targetRenderItem;

	auto it = m_RenderItemSets.find(renderItemSetName);

	assert(it != m_RenderItemSets.end());

	for (auto& it2 : it->second.items)
	{
		if (it2->isRenderOK)
		{
			cmdList->IASetVertexBuffers(0, 1, &it2->geo->GetVertexBufferView());
			cmdList->IASetPrimitiveTopology(it2->primitiveType);

			D3D12_GPU_VIRTUAL_ADDRESS objCBAdress =
				frameObjCB->GetGPUVirtualAddress() + it2->objCBIndex*objCBByteSize;
			cmdList->SetGraphicsRootConstantBufferView(1, objCBAdress);

			if (it2->texture.ptr)
			{
				cmdList->SetGraphicsRootDescriptorTable(0, it2->texture);
			}

			if (it2->mater)
			{
				D3D12_GPU_VIRTUAL_ADDRESS matCBAddress =
					frameMaterCB->GetGPUVirtualAddress() + it2->mater->matCBIndex*matCBByteSize;
				cmdList->SetGraphicsRootConstantBufferView(3, matCBAddress);
			}

			if (it2->isIndexRender)
			{
				cmdList->IASetIndexBuffer(&it2->geo->GetIndexBufferView());
				cmdList->DrawIndexedInstanced(it2->indexCount, 1, it2->startIndexLocation, it2->baseVertexLocation, 0);
			}
			else
			{
				cmdList->DrawInstanced(it2->indexCount, 1, it2->baseVertexLocation, 0);
			}
		}
	}
}

void cRenderItemMG::AddRenderSet(string renderSetKeyName)
{
	auto iter = m_RenderItemSets.find(renderSetKeyName);

	assert(iter == m_RenderItemSets.end());

	if (m_RenderItemSets.size() == 0)
	{
		baseKey = renderSetKeyName;
	}

	m_RenderItemSets[renderSetKeyName];
}

RenderItemSet * cRenderItemMG::GetRenderItemSet(string key)
{
	auto iter = m_RenderItemSets.find(key);
	assert(iter != m_RenderItemSets.end());

	return &iter->second;
}

shared_ptr<RenderItem> cRenderItemMG::AddRenderItem(string renderSetKeyName)
{
	if (renderSetKeyName == "")
	{
		renderSetKeyName = baseKey;
	}

	auto iter = m_RenderItemSets.find(renderSetKeyName);
	assert(iter != m_RenderItemSets.end());

	auto renderItem = make_shared<RenderItem>();
	renderItem->objCBIndex = RenderItemIndexCount++;
	iter->second.items.push_back(renderItem);

	return iter->second.items.back();
}
