#include "stdafx.h"

cMeshgeometryMG* cMeshgeometryMG::instance = nullptr;


cMeshgeometryMG::cMeshgeometryMG()
	: m_needMeshBuildUp(false)
	, m_device(nullptr)
	, m_CommandList(nullptr)
	, m_DirectCmdListAlloc(nullptr)
	, m_CommandQueue(nullptr)
{
}

cMeshgeometryMG::~cMeshgeometryMG()
{
}

void cMeshgeometryMG::Build(ID3D12Device* device, ComPtr<ID3D12CommandQueue> commandQueue)
{
	m_device = device;
	m_CommandQueue = commandQueue;

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_DirectCmdListAlloc.GetAddressOf())));

	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_DirectCmdListAlloc.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	m_CommandList->Close();
}

MeshGeometry* cMeshgeometryMG::AddMeshGeometry(const string& name, const void * vertexData, const void * indexData,
	UINT vertexByteStride, UINT vertexBufferByteSize, DXGI_FORMAT indexFormat, UINT indexBufferByteSize,
	bool leaveDataInCPU, const unordered_map<string, SubMeshGeometry>* subMeshs)
{
	auto iter = m_Meshgometrys.find(name);
	assert(iter == m_Meshgometrys.end());

	if (m_needMeshBuildUp == false)
	{
		ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));
		m_needMeshBuildUp = true;
	}

	unique_ptr<MeshGeometry> geo = make_unique<MeshGeometry>();
	geo->name = name;

	DataInputGeometry(geo.get(), vertexData, indexData, vertexByteStride,
		vertexBufferByteSize, indexFormat, indexBufferByteSize, leaveDataInCPU, subMeshs);

	m_Meshgometrys.insert({ name, move(geo) });

	return m_Meshgometrys[name].get();
}

MeshGeometry * cMeshgeometryMG::AddTemporaryMesh(const string & name)
{
	auto iter = m_Meshgometrys.find(name);
	assert(iter == m_Meshgometrys.end());

	unique_ptr<MeshGeometry> geo = make_unique<MeshGeometry>();
	geo->name = name;

	SubMeshGeometry sub;
	sub.baseVertexLocation = 0;
	sub.startIndexLocation = 0;
	sub.indexCount = 0;
	geo->DrawArgs[name] = sub;

	m_Meshgometrys.insert({ name, move(geo) });

	return m_Meshgometrys[name].get();
}

void cMeshgeometryMG::ChangeMeshGeometryData(const string & name, const void * vertexData, const void * indexData,
	UINT vertexByteStride, UINT vertexBufferByteSize, DXGI_FORMAT indexFormat,
	UINT indexBufferByteSize, bool leaveDataInCPU, const unordered_map<string, SubMeshGeometry>* subMeshs)
{
	auto iter = m_Meshgometrys.find(name);
	assert(iter != m_Meshgometrys.end());

	if (m_needMeshBuildUp == false)
	{
		ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));
		m_needMeshBuildUp = true;
	}

	MeshGeometry* geo = iter->second.get();

	ClearGeometry(geo);

	DataInputGeometry(geo, vertexData, indexData, vertexByteStride,
		vertexBufferByteSize, indexFormat, indexBufferByteSize, leaveDataInCPU, subMeshs);
}

void cMeshgeometryMG::CopyData(MeshGeometry * destGeo, const MeshGeometry * src)
{
	assert(destGeo != src);
	assert(destGeo);
	assert(destGeo->name != "");

	if (m_needMeshBuildUp == false)
	{
		ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));
		m_needMeshBuildUp = true;
	}

	ClearGeometry(destGeo);

	assert(src->vertexBufferCPU);
	DataInputGeometry(destGeo, src->vertexBufferCPU->GetBufferPointer(), src->indexBufferCPU->GetBufferPointer(),
		src->vertexByteStride, src->vertexBufferByteSize,
		src->indexFormat, src->indexBufferByteSize, false, &src->DrawArgs);

	if (destGeo->DrawArgs.size() == 1)
	{
		auto drawArg = destGeo->DrawArgs.begin()->second;
		destGeo->DrawArgs.clear();
		destGeo->DrawArgs.insert({ destGeo->name, drawArg });
	}
}

void cMeshgeometryMG::MeshBuildUpGPU(ComPtr<ID3D12Fence> fence, UINT64 & currentFenc)
{
	if (m_needMeshBuildUp)
	{
		FlushCommandQueue(fence, currentFenc);

		ThrowIfFailed(m_CommandList->Close());
		ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
		m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

		FlushCommandQueue(fence, currentFenc);
		DisposUploadBuffers();

		m_needMeshBuildUp = false;
	}
}

void cMeshgeometryMG::FlushCommandQueue(ComPtr<ID3D12Fence> fence, UINT64 & currentFenc)
{
	currentFenc++;

	ThrowIfFailed(m_CommandQueue->Signal(fence.Get(), currentFenc));

	if (fence->GetCompletedValue() < currentFenc)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		ThrowIfFailed(fence->SetEventOnCompletion(currentFenc, eventHandle));

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void cMeshgeometryMG::ClearGeometry(MeshGeometry * geo)
{
	geo->vertexBufferCPU = nullptr;
	geo->indexBufferCPU = nullptr;
	geo->vertexBufferGPU = nullptr;
	geo->indexBufferGPU = nullptr;
	geo->vertexUploadBuffer = nullptr;
	geo->indexUploadBuffer = nullptr;
	geo->octree = nullptr;
	geo->DrawArgs.clear();
}

void cMeshgeometryMG::DataInputGeometry(MeshGeometry * geo, const void * vertexData, const void * indexData, UINT vertexByteStride, UINT vertexBufferByteSize, DXGI_FORMAT indexFormat, UINT indexBufferByteSize, bool leaveDataInCPU, const unordered_map<string, SubMeshGeometry>* subMeshs)
{
	geo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_device, m_CommandList.Get(),
		vertexData, vertexBufferByteSize, geo->vertexUploadBuffer);

	geo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(m_device, m_CommandList.Get(),
		indexData, indexBufferByteSize, geo->indexUploadBuffer);

	if (leaveDataInCPU)
	{
		D3DCreateBlob(vertexBufferByteSize, geo->vertexBufferCPU.GetAddressOf());
		memcpy(geo->vertexBufferCPU->GetBufferPointer(), vertexData, vertexBufferByteSize);

		D3DCreateBlob(indexBufferByteSize, geo->indexBufferCPU.GetAddressOf());
		memcpy(geo->indexBufferCPU->GetBufferPointer(), indexData, indexBufferByteSize);
	}

	geo->indexFormat = indexFormat;
	geo->indexBufferByteSize = indexBufferByteSize;
	geo->vertexBufferByteSize = vertexBufferByteSize;
	geo->vertexByteStride = vertexByteStride;

	if (subMeshs == nullptr)
	{
		SubMeshGeometry sub;
		sub.baseVertexLocation = 0;
		sub.startIndexLocation = 0;
		if (indexFormat == DXGI_FORMAT_R16_UINT)
		{
			sub.indexCount = indexBufferByteSize / sizeof(UINT16);
		}
		else if (indexFormat == DXGI_FORMAT_R32_UINT)
		{
			sub.indexCount = indexBufferByteSize / sizeof(UINT32);
		}

		geo->DrawArgs[geo->name] = sub;
	}
	else
	{
		geo->DrawArgs = *subMeshs;
	}
}


void cMeshgeometryMG::DisposUploadBuffers()
{
	for (auto& it : m_Meshgometrys)
	{
		it.second->DisPosUploaders();
	}
}

MeshGeometry * cMeshgeometryMG::GetMeshGeometry(string name)
{
	auto iter = m_Meshgometrys.find(name);

	if (iter != m_Meshgometrys.end())
	{
		return iter->second.get();
	}

	return false;
}
