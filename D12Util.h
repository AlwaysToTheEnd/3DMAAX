#pragma once

class cOcTree;

struct PICKRAY
{
	XMVECTOR origin;
	XMVECTOR ray;
};

inline static XMFLOAT3 operator-(XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	return { lhs.x - rhs.x,lhs.y - rhs.y,lhs.z - rhs.z };
}


inline static XMFLOAT3 operator+(XMFLOAT3& lhs, XMFLOAT3& rhs)
{
	return { lhs.x + rhs.x,lhs.y + rhs.y,lhs.z + rhs.z };
}

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
	if (obj)
	{
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
	}
}

static XMFLOAT3 operator *(const XMFLOAT3& lhs,float rhs)
{
	return { lhs.x*rhs,lhs.y*rhs ,lhs.z*rhs };
}

class d3dUtil
{
public:

	static bool IsKeyDown(int vkeyCode);

	static string ToString(HRESULT hr);

	static UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		return (byteSize + 255) & ~255;
	}

	static ComPtr<ID3DBlob> LoadBinary(const wstring& filename);

	static ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer);

	static ComPtr<ID3DBlob> CompileShader(
		const wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const string& entrypoint,
		const string& target);
};

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const wstring& functionName, const wstring& filename, int lineNumber);

	wstring ToString()const;

	HRESULT ErrorCode = S_OK;
	wstring FunctionName;
	wstring Filename;
	int LineNumber = -1;
};

inline wstring AnsiToWString(const string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif


//------------------------------------user define Structers--------------------//
struct Material
{
	string name;

	int matCBIndex = -1;
	int diffuseSrvHeapIndex = -1;

	int numFramesDirty = gNumFrameResources;

	XMFLOAT4 diffuseAlbedo = { 1,1,1,1 };
	XMFLOAT3 fresnel0 = { 0.01f,0.01f,0.01f };

	float roughness = 0.25f;
	XMFLOAT4X4 matTransform = MathHelper::Identity4x4();
};

struct MaterialConstants
{
	XMFLOAT4 diffuseAlbedo = { 1,1,1,1 };
	XMFLOAT3 fresnel0 = { 0.01f,0.01f,0.01f };

	float roughness = 0.25f;
	XMFLOAT4X4 matTransform = MathHelper::Identity4x4();
};

struct SubMeshGeometry
{
	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	UINT baseVertexLocation = 0;

	BoundingBox bounds;
};

struct MeshGeometry
{
	string name;

	ComPtr<ID3DBlob> vertexBufferCPU = nullptr;
	ComPtr<ID3DBlob> indexBufferCPU = nullptr;

	ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource> indexBufferGPU = nullptr;

	ComPtr<ID3D12Resource> vertexUploadBuffer = nullptr;
	ComPtr<ID3D12Resource> indexUploadBuffer = nullptr;

	UINT vertexByteStride = 0;
	UINT vertexBufferByteSize = 0;
	DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
	UINT indexBufferByteSize = 0;

	unique_ptr<cOcTree> octree = nullptr;
	unordered_map<string, SubMeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = vertexByteStride;
		vbv.SizeInBytes = vertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView()
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = indexFormat;
		ibv.SizeInBytes = indexBufferByteSize;

		return ibv;
	}

	void DisPosUploaders()
	{
		vertexUploadBuffer = nullptr;
		indexUploadBuffer = nullptr;
	}

	void SetOctree(int recursion);
};

struct Light
{
	XMFLOAT3 strength = { 0.5f,0.5f,0.5f };
	float falloffStart = 1.0f;
	XMFLOAT3 direction = { 0,-1.0f,0 };
	float falloffEnd = 10.0f;
	XMFLOAT3 position = { 0,0,0 };
	float spotPower = 64.0f;
};

struct Texture
{
	string name;

	ComPtr<ID3D12Resource> resource = nullptr;
	ComPtr<ID3D12Resource> uploadHeap = nullptr;
};