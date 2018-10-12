#pragma once

#define RENDERITEMMG cRenderItemMG::Get()

struct RenderItemSet
{
	list<shared_ptr<RenderItem>> items;
};

class cRenderItemMG
{
public:
	static cRenderItemMG * Get()
	{
		if (instance == nullptr)
		{
			instance = new cRenderItemMG;
		}

		return instance;
	}

	void Update(FrameResource* currFrameResource);
	void Render(ID3D12GraphicsCommandList * cmdList, 
		FrameResource* currFrameResource, string renderItemSetName);
	void AddRenderSet(string renderSetKeyName);
	RenderItemSet* GetRenderItemSet(string key);

public:
	shared_ptr<RenderItem> AddRenderItem(string renderSetKeyName = "");
	UINT GetNumRenderItems() { return RenderItemIndexCount; };

private:
	cRenderItemMG() = default;

	static cRenderItemMG* instance;
	string baseKey;
	UINT RenderItemIndexCount=0;
	concurrency::concurrent_unordered_map <string, RenderItemSet> m_RenderItemSets;
};

