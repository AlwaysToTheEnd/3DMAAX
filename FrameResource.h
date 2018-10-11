#pragma once

#define MAXLIGHTS 16

struct ObjectConstants
{
	XMFLOAT4X4 world = MathHelper::Identity4x4();
	XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};

struct PassConstants
{
	XMFLOAT4X4 view = MathHelper::Identity4x4();
	XMFLOAT4X4 invView= MathHelper::Identity4x4();
	XMFLOAT4X4 proj = MathHelper::Identity4x4();
	XMFLOAT4X4 invProj = MathHelper::Identity4x4();
	XMFLOAT4X4 viewProj = MathHelper::Identity4x4();
	XMFLOAT4X4 invViewProj = MathHelper::Identity4x4();
	XMFLOAT3 eyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	XMFLOAT2 renderTargetSize = { 0.0f, 0.0f };
	XMFLOAT2 invRenderTargetSize = { 0.0f, 0.0f };
	float nearZ = 0.0f;
	float farZ = 0.0f;
	float totalTime = 0.0f;
	float deltaTime = 0.0f;

	XMFLOAT4 ambientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	Light Lights[MAXLIGHTS];
};

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
};

struct FrameResource
{
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount,
		UINT materialCount)
	{
		ThrowIfFailed(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdListAlloc.GetAddressOf())));

		passCB = make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
		materialCB = make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
		objectCB = make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	}

	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;

	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<PassConstants>> passCB = nullptr;
	unique_ptr<UploadBuffer<MaterialConstants>> materialCB = nullptr;
	unique_ptr<UploadBuffer<ObjectConstants>> objectCB = nullptr;


	UINT64 fence = 0;
};

struct RenderItem
{
	RenderItem()
	{
		texture.ptr = 0;
	}

	XMFLOAT4X4 world = MathHelper::Identity4x4();
	XMFLOAT4X4 texTransform = MathHelper::Identity4x4();

	int numFramesDirty = gNumFrameResources;
	bool isIndexRender = true;
	bool isRenderOK = true;

	int objCBIndex = -1;

	Material* mater = nullptr;
	MeshGeometry* geo = nullptr;
	D3D12_GPU_DESCRIPTOR_HANDLE texture;

	D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT indexCount = 0;
	UINT startIndexLocation = 0;
	int baseVertexLocation = 0;
};

enum class RenderLayer :int
{
	Opaque=0,
	Count
};