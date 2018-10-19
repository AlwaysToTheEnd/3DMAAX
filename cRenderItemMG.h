#pragma once

#define RENDERITEMMG RenderItemMG::Get()

struct RenderInstance
{
	InstanceData instanceData;
	int numFramesDirty = gNumFrameResources;
	bool m_isRenderOK = true;
};

class cRenderItem
{
public:
	static void SetDevice(ID3D12Device* device) { m_device = device; }

	void SetPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY type) { m_primitiveType = type; }
	void SetGeometry(const MeshGeometry* geometry, string submeshName);
	void SetRenderOK(bool value) { m_isRenderOK = value; }

	shared_ptr<RenderInstance> GetRenderIsntance();

private:
	friend class RenderItemMG;

	void Update();
	void Render(ID3D12GraphicsCommandList * cmdList);
	void SetUploadBufferSize(UINT numInstance);

private:
	static ID3D12Device* m_device;

	unique_ptr<UploadBuffer<InstanceData>> m_objectCB[gNumFrameResources] = {};
	UploadBuffer<InstanceData>* m_currFrameCB = nullptr;
	UINT m_currFrameCBIndex = 0;
	UINT m_currBufferSize = 4;
	int m_numFrameDirty = gNumFrameResources;

	bool m_isRenderOK = true;
	UINT m_renderInstanceCount = 0;

	const MeshGeometry* m_geo = nullptr;
	D3D12_PRIMITIVE_TOPOLOGY m_primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_indexCount = 0;
	UINT m_startIndexLocation = 0;
	UINT m_baseVertexLocation = 0;

	list<shared_ptr<RenderInstance>> m_instances;
};

struct RenderItemSet
{
	list<shared_ptr<cRenderItem>> items;
};

class RenderItemMG
{
public:
	static RenderItemMG * Get()
	{
		if (instance == nullptr)
		{
			instance = new RenderItemMG;
		}

		return instance;
	}

	void Update();
	void Render(ID3D12GraphicsCommandList * cmdList, string renderItemSetName);
	void AddRenderSet(string renderSetKeyName);
	RenderItemSet* GetRenderItemSet(string key);

public:
	shared_ptr<cRenderItem> AddRenderItem(string renderSetKeyName);

private:
	RenderItemMG() = default;

	static RenderItemMG* instance;
	string baseKey;
	concurrency::concurrent_unordered_map <string, RenderItemSet> m_RenderItemSets;
};

