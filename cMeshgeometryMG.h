#pragma once

#define MESHMG cMeshgeometryMG::Get()

class cMeshgeometryMG
{
public:
	~cMeshgeometryMG();

	static cMeshgeometryMG * Get()
	{
		if (instance == nullptr)
		{
			instance = new cMeshgeometryMG;
		}

		return instance;
	}

	void Build(ID3D12Device* device ,ComPtr<ID3D12CommandQueue> commandQueue);

	MeshGeometry* AddMeshGeometry(const string& name,const void* vertexData,const void* indexData, UINT vertexByteStride,
		UINT vertexBufferByteSize, DXGI_FORMAT indexFormat, UINT indexBufferByteSize, bool leaveDataInCPU,
		const unordered_map<string, SubMeshGeometry>* subMeshs=nullptr);

	MeshGeometry* AddTemporaryMesh(const string& name);

	void ChangeMeshGeometryData(const string& name, const void* vertexData, const void* indexData, UINT vertexByteStride,
		UINT vertexBufferByteSize, DXGI_FORMAT indexFormat, UINT indexBufferByteSize, bool leaveDataInCPU,
		const unordered_map<string, SubMeshGeometry>* subMeshs = nullptr);

	void CopyData(MeshGeometry* destGeo, const MeshGeometry* src, bool isLeaveCPUData=false);

	void MeshBuildUpGPU(ComPtr<ID3D12Fence> fence, UINT64& currentFenc);
	void DisposUploadBuffers();

	MeshGeometry* GetMeshGeometry(string name);

private:
	cMeshgeometryMG();
	void FlushCommandQueue(ComPtr<ID3D12Fence> fence, UINT64& currentFenc);

private:
	static cMeshgeometryMG*				instance;
	bool								m_needMeshBuildUp;
	ID3D12Device*						m_device;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	ComPtr<ID3D12CommandAllocator>		m_DirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;
	unordered_map<string, unique_ptr<MeshGeometry>> m_Meshgometrys;
};

//m_CurrentFence++;
//
//ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence));
//
//if (m_Fence->GetCompletedValue() < m_CurrentFence)
//{
//	HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
//
//	ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrentFence, eventHandle));
//
//	WaitForSingleObject(eventHandle, INFINITE);
//	CloseHandle(eventHandle);
//}