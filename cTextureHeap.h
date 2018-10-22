#pragma once

class cTextureHeap
{
public:
	cTextureHeap() = delete;
	cTextureHeap(cTextureHeap& rhs) = delete;
	cTextureHeap& operator=(const cTextureHeap& rhs) = delete;
	cTextureHeap(ID3D12Device* device ,UINT maxTexture);

	void AddTexture(ID3D12CommandQueue* cmdqueue, string name, wstring filename);
	D3D12_GPU_DESCRIPTOR_HANDLE GetTexture(string name);
	ID3D12DescriptorHeap* GetHeap() { return m_SrvHeap.Get(); }

private:
	struct TEXTURENUM
	{
		UINT num=0;
		Texture tex;
	};

	ID3D12Device* m_device;
	UINT m_SrvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_SrvHeap;
	unordered_map<string, TEXTURENUM> m_Textures;
};