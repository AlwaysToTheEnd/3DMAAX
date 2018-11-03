#pragma once

#define MAXLIGHTS 16

struct InstanceData
{
	XMFLOAT4X4	World = MathHelper::Identity4x4();
	XMFLOAT4X4	TexTransform = MathHelper::Identity4x4();
	float		sizeScale = 1;
	UINT		MaterialIndex = 0;
	UINT		PickState = 0;
	UINT		InstancePad2 = 0;
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

struct NT_Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;

	NT_Vertex() {}
	NT_Vertex(XMFLOAT3 _pos, XMFLOAT3 _normal, XMFLOAT2 _uv)
	{
		pos = _pos;
		normal = _normal;
		uv = _uv;
	}
};

struct C_Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;

	C_Vertex() {}

	C_Vertex(XMFLOAT3 _pos, XMFLOAT4 _color)
	{
		pos = _pos;
		color = _color;
	}

	C_Vertex(XMFLOAT3 _pos, XMVECTOR _color)
	{
		pos = _pos;
		XMStoreFloat4(&color, _color);
	}
};

struct FrameResource
{
	FrameResource(ID3D12Device* device, UINT passCount, UINT materialCount)
	{
		ThrowIfFailed(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdListAlloc.GetAddressOf())));

		passCB = make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
		materialBuffer = make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, false);
	}

	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;

	ComPtr<ID3D12CommandAllocator> cmdListAlloc;

	unique_ptr<UploadBuffer<PassConstants>> passCB = nullptr;
	unique_ptr<UploadBuffer<MaterialConstants>> materialBuffer = nullptr;

	UINT64 fence = 0;
};