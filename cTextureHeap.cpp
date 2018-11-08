#include "stdafx.h"


cTextureHeap::cTextureHeap(ID3D12Device * device, UINT maxTexture)
	: m_SrvDescriptorSize(0)
{
	assert(device && maxTexture);

	m_device = device;
	m_SrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = maxTexture;

	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SrvHeap.GetAddressOf())));
}

void cTextureHeap::AddTexture(ID3D12CommandQueue* cmdqueue, string name, wstring filename)
{
	auto it = m_Textures.find(name);
	assert(it == m_Textures.end() && "This name is overlapping name");

	TEXTURENUM addTexture;
	addTexture.num = (UINT)m_Textures.size();
	addTexture.tex.name = name;

	ResourceUploadBatch resourceUpload(m_device);
	resourceUpload.Begin();

	size_t index = filename.find('.') + 1;
	wstring extension;
	extension.assign(&filename[index], filename.size() - index);

	if (extension == L"dds")
	{
		ThrowIfFailed(CreateDDSTextureFromFile(m_device,
			resourceUpload, filename.c_str(), addTexture.tex.resource.GetAddressOf()));
		m_Textures[name] = addTexture;
	}
	else
	{
		ThrowIfFailed(CreateWICTextureFromFile(m_device,
			resourceUpload, filename.c_str(), addTexture.tex.resource.GetAddressOf()));
		m_Textures[name] = addTexture;
	}

	auto uploadResourceFinished = resourceUpload.End(cmdqueue);
	uploadResourceFinished.wait();

	auto srvHeapHandle = (CD3DX12_CPU_DESCRIPTOR_HANDLE)m_SrvHeap->GetCPUDescriptorHandleForHeapStart();
	srvHeapHandle.Offset(addTexture.num, m_SrvDescriptorSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = addTexture.tex.resource->GetDesc().Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	srvDesc.Texture2D.MipLevels = addTexture.tex.resource->GetDesc().MipLevels;

	m_device->CreateShaderResourceView(addTexture.tex.resource.Get(), &srvDesc, srvHeapHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE cTextureHeap::GetTexture(string name)
{
	auto it = m_Textures.find(name);
	assert(it != m_Textures.end() && "can not find this name");

	auto srvHeapHandle = (CD3DX12_GPU_DESCRIPTOR_HANDLE)m_SrvHeap->GetGPUDescriptorHandleForHeapStart();
	srvHeapHandle.Offset(it->second.num, m_SrvDescriptorSize);

	return srvHeapHandle;
}