#include "stdafx.h"


cTextureHeap::cTextureHeap(ID3D12Device * device, UINT maxTexture)
	: m_SrvDescriptorSize(0)
{
	assert(device && maxTexture);

	m_device = device;
	m_SrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NodeMask = 0;
	srvHeapDesc.NumDescriptors = maxTexture;

	ThrowIfFailed(device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_SrvHeap.GetAddressOf())));
}

void cTextureHeap::AddTexture(ID3D12GraphicsCommandList* cmdList, string name, wstring filename)
{
	auto it = m_Textures.find(name);
	assert(it == m_Textures.end() && "This name is overlapping name");

	TEXTURENUM addTexture;
	addTexture.num = m_Textures.size();
	addTexture.tex.name = name;

	ThrowIfFailed(CreateDDSTextureFromFile(m_device,
		cmdList, filename.c_str(),
		addTexture.tex.resource, addTexture.tex.uploadHeap));
	m_Textures[name] = addTexture;

	auto srvHeapHandle = (CD3DX12_CPU_DESCRIPTOR_HANDLE)m_SrvHeap->GetCPUDescriptorHandleForHeapStart();
	srvHeapHandle.Offset(addTexture.num, m_SrvDescriptorSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = addTexture.tex.resource->GetDesc().Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	
	m_device->CreateShaderResourceView(addTexture.tex.resource.Get(), &srvDesc, srvHeapHandle);
}

void cTextureHeap::AddTexture(ID3D12Resource * resource, string name)
{
	auto it = m_Textures.find(name);
	assert(it == m_Textures.end() && "This name is overlapping name");

	TEXTURENUM addTexture;
	addTexture.num = m_Textures.size();
	addTexture.tex.name = name;
	addTexture.tex.resource = resource;

	auto srvHeapHandle = (CD3DX12_CPU_DESCRIPTOR_HANDLE)m_SrvHeap->GetCPUDescriptorHandleForHeapStart();
	srvHeapHandle.Offset(addTexture.num, m_SrvDescriptorSize);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = addTexture.tex.resource->GetDesc().Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	m_device->CreateShaderResourceView(addTexture.tex.resource.Get(), &srvDesc, srvHeapHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE cTextureHeap::GetTexture(string name)
{
	auto it = m_Textures.find(name);
	assert(it != m_Textures.end()&&"can not find this name");

	auto srvHeapHandle = (CD3DX12_GPU_DESCRIPTOR_HANDLE)m_SrvHeap->GetGPUDescriptorHandleForHeapStart();
	srvHeapHandle.Offset(it->second.num, m_SrvDescriptorSize);

	return srvHeapHandle;
}

//D3D12_GPU_DESCRIPTOR_HANDLE cTextureHeap::GetTexture(string name)
//{
//	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHeapHandle;
//	srvHeapHandle.ptr = 0;
//	
//	auto it = m_Textures.find(name);
//
//	if (it != m_Textures.end())
//	{
//		srvHeapHandle = m_SrvHeap->GetGPUDescriptorHandleForHeapStart();
//		srvHeapHandle.Offset(it->second.num, m_SrvDescriptorSize);
//	}
//
//	return srvHeapHandle;
//}
