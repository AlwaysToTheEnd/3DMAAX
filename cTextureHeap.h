#pragma once

class cTextureHeap
{
public:
	cTextureHeap() = delete;
	cTextureHeap(cTextureHeap& rhs) = delete;
	cTextureHeap& operator=(const cTextureHeap& rhs) = delete;
	cTextureHeap(ID3D12Device* device ,UINT maxTexture);

	void AddTexture(ID3D12CommandQueue* cmdqueue, const string& name, const wstring& filename);
	void AddCubeMapTexture(ID3D12CommandQueue* cmdqueue, const string& name, const wstring& filename);
	void AddNullTexture(const string& name, DXGI_FORMAT srvFormat,
		const D3D12_RESOURCE_DESC* resourceDesc,const D3D12_CLEAR_VALUE* optClear);

	ID3D12DescriptorHeap* GetHeap() { return m_SrvHeap.Get(); }
	ID3D12Resource* GetTexture(const string& name);
	
	UINT GetTextureIndex(const string& name) const;
	UINT GetTexturesNum() const { return (UINT)m_Textures.size(); }

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