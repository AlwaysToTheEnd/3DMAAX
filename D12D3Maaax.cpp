#include "stdafx.h"

D12D3Maaax::D12D3Maaax(HINSTANCE hInstance)
	:D12App(hInstance)
{

}

D12D3Maaax::~D12D3Maaax()
{
	m_operator = nullptr;
	m_TextureHeap = nullptr;
	m_cubeMapRenderInstance = nullptr;
	m_cubeMapRender = nullptr;
	delete UIMG;
	delete MESHMG;
	delete RENDERITEMMG;
	delete OBJCOORD;
	delete INPUTMG;

	if (m_D3dDevice != nullptr)
		FlushCommandQueue();

	delete FONTMANAGER;
}

bool D12D3Maaax::Initialize()
{
	if (!D12App::Initialize())
		return false;

	cRenderItem::SetDevice(m_D3dDevice.Get());

	FONTMANAGER->Build(m_D3dDevice.Get(), m_CommandQueue.Get());
	MESHMG->Build(m_D3dDevice.Get(), m_CommandQueue, m_Fence, &m_CurrentFence);
	ThrowIfFailed(m_CommandList->Reset(m_DirectCmdListAlloc.Get(), nullptr));
	BuildTextures();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPSOs();
	BuildGeometry();
	BuildMaterials();
	BuildObjects();
	BuildFrameResources();
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();

	MESHMG->MeshBuildUpGPU();

	OnResize();

	return true;
}

void D12D3Maaax::OnResize()
{
	D12App::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_3DProj, P);
	P = XMMatrixOrthographicOffCenterLH(0, (float)m_ClientWidth, (float)m_ClientHeight, 0, -1, 1);
	XMStoreFloat4x4(&m_2DProj, P);

	FONTMANAGER->Resize(m_ClientWidth, m_ClientHeight);
}

void D12D3Maaax::Update()
{
	m_CurrFrameResourceIndex = (m_CurrFrameResourceIndex + 1) % gNumFrameResources;
	m_CurrFrameResource = m_FrameResources[m_CurrFrameResourceIndex].get();

	if (m_CurrFrameResource->fence != 0 && m_Fence->GetCompletedValue() < m_CurrFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	m_Camera.Update();
	UpdateShadowMatrix();
	UpdateMainPassCB();
	INPUTMG->Update(m_Camera.GetEyePos(), *m_Camera.GetViewMatrix(), m_3DProj, (float)m_ClientWidth, (float)m_ClientHeight);
	UIMG->Update();

	m_operator->Update();
	OBJCOORD->Update();

	MESHMG->MeshBuildUpGPU();
	RENDERITEMMG->Update();
	UpdateMaterialCBs();

	if (GetAsyncKeyState(VK_F2) & 0x0001)
	{
		m_isBaseWireFrameMode = !m_isBaseWireFrameMode;
	}
}

void D12D3Maaax::Draw()
{
	auto cmdListAlloc = m_CurrFrameResource->cmdListAlloc.Get();

	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(m_CommandList->Reset(cmdListAlloc,
		m_PSOs["cubeMap"].Get()));

	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Gray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	/////////////////////////////
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_TextureHeap->GetHeap() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	auto matBuffer = m_CurrFrameResource->materialBuffer->Resource();
	m_CommandList->SetGraphicsRootShaderResourceView(1, matBuffer->GetGPUVirtualAddress());

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = m_TextureHeap->GetHeap()->GetGPUDescriptorHandleForHeapStart();
	m_CommandList->SetGraphicsRootDescriptorTable(3, srvHandle);
	srvHandle.ptr += m_CBV_SRV_UAV_DescriptorSize * m_TextureHeap->GetTextureIndex("cubeMap");
	m_CommandList->SetGraphicsRootDescriptorTable(4, srvHandle);
	srvHandle.ptr += m_CBV_SRV_UAV_DescriptorSize;
	m_CommandList->SetGraphicsRootDescriptorTable(5, srvHandle);
	/////////////////////////////

	auto passCBAddress = m_CurrFrameResource->passCB->Resource()->GetGPUVirtualAddress();
	m_CommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);

	RENDERITEMMG->Render(m_CommandList.Get(), "cubeMap");

	if (m_isBaseWireFrameMode)
	{
		m_CommandList->SetPipelineState(m_PSOs["baseWF"].Get());
	}
	else
	{
		DrawShadowMap();
		m_CommandList->SetPipelineState(m_PSOs["base"].Get());
	}

	RENDERITEMMG->Render(m_CommandList.Get(), "base");
	RENDERITEMMG->Render(m_CommandList.Get(), cMesh::m_meshRenderName);

	m_CommandList->SetPipelineState(m_PSOs["drawLine"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), "line");

	m_CommandList->SetPipelineState(m_PSOs["drawElement"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), "dot");
	RENDERITEMMG->Render(m_CommandList.Get(), "objectCoordinator");

	m_CommandList->SetPipelineState(m_PSOs["planes"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), "plane");

	passCBAddress += d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	m_CommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);

	m_CommandList->SetPipelineState(m_PSOs["ui"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), "ui");

	////////////////////////////////

	FONTMANAGER->Render(m_CommandList.Get());
	FONTMANAGER->Commit(m_CommandQueue.Get());

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	ThrowIfFailed(m_CommandList->Close());

	ID3D12CommandList* cmdsLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(m_SwapChain->Present(0, 0));
	m_CurrBackBuffer = (m_CurrBackBuffer + 1) % SwapChainBufferCount;

	m_CurrFrameResource->fence = ++m_CurrentFence;

	m_CommandQueue->Signal(m_Fence.Get(), m_CurrentFence);
}


void D12D3Maaax::UpdateMaterialCBs()
{
	auto currMaterialCB = m_CurrFrameResource->materialBuffer.get();

	for (auto& e : m_Materials)
	{
		Material* mat = e.second.get();
		if (mat->numFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->matTransform);

			MaterialConstants matConstants;
			matConstants.diffuseAlbedo = mat->diffuseAlbedo;
			matConstants.fresnel0 = mat->fresnel0;
			matConstants.roughness = mat->roughness;
			matConstants.diffuseMapIndex = mat->diffuseMapIndex;
			XMStoreFloat4x4(&matConstants.matTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->matCBIndex, matConstants);

			mat->numFramesDirty--;
		}
	}
}

void D12D3Maaax::UpdateShadowMatrix()
{
	const float sceneSphereRadius = m_Camera.GetDistance();

	XMVECTOR lightDir = XMVector3Normalize(XMLoadFloat3(&m_MainPassCB.Lights[0].direction));
	XMVECTOR targetPos = XMLoadFloat3(&m_Camera.GetTargetPos());
	XMVECTOR lightPos = (targetPos -(1.0f*sceneSphereRadius*lightDir));
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	float l = sphereCenterLS.x - sceneSphereRadius;
	float b = sphereCenterLS.y - sceneSphereRadius;
	float n = sphereCenterLS.z - sceneSphereRadius;
	float r = sphereCenterLS.x + sceneSphereRadius;
	float t = sphereCenterLS.y + sceneSphereRadius;
	float f = sphereCenterLS.z + sceneSphereRadius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj* T;
	XMStoreFloat4x4(&m_ShadowMapMatrix, S);
	XMStoreFloat4x4(&m_RightVeiwProjMatrix, lightView*lightProj);
}

void D12D3Maaax::UpdateMainPassCB()
{
	auto currPassCB = m_CurrFrameResource->passCB.get();

	XMMATRIX view = XMLoadFloat4x4(m_Camera.GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&m_3DProj);
	XMMATRIX rightViewProjMatrix = XMLoadFloat4x4(&m_RightVeiwProjMatrix);
	XMMATRIX shadowMapMatrix = XMLoadFloat4x4(&m_ShadowMapMatrix);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&m_MainPassCB.view, XMMatrixTranspose(view));
	XMStoreFloat4x4(&m_MainPassCB.invView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&m_MainPassCB.proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&m_MainPassCB.invProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&m_MainPassCB.viewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&m_MainPassCB.invViewProj, XMMatrixTranspose(invViewProj));
	XMStoreFloat4x4(&m_MainPassCB.rightViewProj, XMMatrixTranspose(rightViewProjMatrix));
	XMStoreFloat4x4(&m_MainPassCB.shadowMapMatrix, XMMatrixTranspose(shadowMapMatrix));
	m_MainPassCB.eyePosW = m_Camera.GetEyePos();
	m_MainPassCB.renderTargetSize = XMFLOAT2((float)m_ClientWidth, (float)m_ClientHeight);
	m_MainPassCB.invRenderTargetSize = XMFLOAT2(1.0f / m_ClientWidth, 1.0f / m_ClientHeight);
	m_MainPassCB.nearZ = 1.0f;
	m_MainPassCB.farZ = 1000.0f;
	m_MainPassCB.totalTime = m_Timer.TotalTime();
	m_MainPassCB.deltaTime = m_Timer.DeltaTime();
	m_MainPassCB.ambientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	m_MainPassCB.Lights[0].direction = { 0.57735f, -0.57735f, 0.57735f };
	m_MainPassCB.Lights[0].strength = { 0.9f, 0.9f, 0.9f };
	m_MainPassCB.Lights[1].direction = { -0.57735f, -0.57735f, 0.57735f };
	m_MainPassCB.Lights[1].strength = { 0.5f, 0.5f, 0.5f };
	m_MainPassCB.Lights[2].direction = { 0.0f, -0.707f, -0.707f };
	m_MainPassCB.Lights[2].strength = { 0.2f, 0.2f, 0.2f };

	currPassCB->CopyData(0, m_MainPassCB);

	proj = XMLoadFloat4x4(&m_2DProj);
	XMStoreFloat4x4(&m_MainPassCB.proj, XMMatrixTranspose(proj));
	m_MainPassCB.view = MathHelper::Identity4x4();
	m_MainPassCB.invView = MathHelper::Identity4x4();
	m_MainPassCB.invProj = MathHelper::Identity4x4();
	m_MainPassCB.viewProj = m_MainPassCB.proj;
	m_MainPassCB.invViewProj = MathHelper::Identity4x4();

	currPassCB->CopyData(1, m_MainPassCB);
}

void D12D3Maaax::DrawShadowMap()
{
	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureHeap->GetTexture("shadowMap").Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_ShadowDsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_CommandList->ClearDepthStencilView(dsvHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	m_CommandList->OMSetRenderTargets(0, nullptr, false, &dsvHandle);

	m_CommandList->SetPipelineState(m_PSOs["shadowMap"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), "base");
	RENDERITEMMG->Render(m_CommandList.Get(), cMesh::m_meshRenderName);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_TextureHeap->GetTexture("shadowMap").Get(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));

	m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
}

void D12D3Maaax::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE cubeMapTexTable;
	cubeMapTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE shadowMapTexTable;
	shadowMapTexTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)m_TextureNames.size(), 2);

	CD3DX12_ROOT_PARAMETER slotRootParam[6];
	slotRootParam[0].InitAsShaderResourceView(0, 1);
	slotRootParam[1].InitAsShaderResourceView(1, 1);
	slotRootParam[2].InitAsConstantBufferView(0);
	slotRootParam[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParam[4].InitAsDescriptorTable(1, &cubeMapTexTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParam[5].InitAsDescriptorTable(1, &shadowMapTexTable, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootDesc;
	rootDesc.Init(6, slotRootParam, (UINT)staticSamplers.size(),
		staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> error = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), error.GetAddressOf());

	if (error != nullptr)
	{
		::OutputDebugStringA((char*)error->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_D3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}

void D12D3Maaax::BuildShadersAndInputLayout()
{
	m_Shaders["baseVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["basePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["uiVS"] = d3dUtil::CompileShader(L"Shaders\\UIShader.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["uiPS"] = d3dUtil::CompileShader(L"Shaders\\UIShader.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["drawElementVS"] = d3dUtil::CompileShader(L"Shaders\\DrawElementSahder.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["drawElementPS"] = d3dUtil::CompileShader(L"Shaders\\DrawElementSahder.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["cubeMapVS"] = d3dUtil::CompileShader(L"Shaders\\CubeMapShader.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["cubeMapPS"] = d3dUtil::CompileShader(L"Shaders\\CubeMapShader.hlsl", nullptr, "PS", "ps_5_1");

	m_Shaders["shadowMapVS"] = d3dUtil::CompileShader(L"Shaders\\ShadowShader.hlsl", nullptr, "VS", "vs_5_1");
	m_Shaders["shadowMapPS"] = d3dUtil::CompileShader(L"Shaders\\ShadowShader.hlsl", nullptr, "PS", "ps_5_1");

	m_NTVertexInputLayout =
	{
		{ "POSITION" ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	m_CVertexInputLayout =
	{
		{ "POSITION" ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "COLOR" ,0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};
}

void D12D3Maaax::BuildGeometry()
{
	cDrawPlane::MeshSetUp();
	cDrawLines::MeshSetUp();
	cDrawDot::MeshSetUp();
	cObjectCoordinator::MeshSetUp();

	GeometryGenerator gen;
	vector<NT_Vertex> vertices;

	GeometryGenerator::MeshData sphere = gen.CreateSphere(0.5f, 30, 30);
	vertices.resize(sphere.Vertices.size());

	for (int i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i].pos = sphere.Vertices[i].Position;
		vertices[i].normal = sphere.Vertices[i].Normal;
		vertices[i].uv = sphere.Vertices[i].TexC;
	}

	vector<UINT16> indices = sphere.GetIndices16();
	m_cubeMapSphere = MESHMG->AddMeshGeometry("cubeMap", vertices.data(), indices.data(),
		sizeof(NT_Vertex), sizeof(NT_Vertex)* (UINT)vertices.size(),
		DXGI_FORMAT_R16_UINT, sizeof(UINT16)*(UINT)indices.size(), false);

	UIMG->Build();
}

void D12D3Maaax::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { m_NTVertexInputLayout.data(), (UINT)m_NTVertexInputLayout.size() };
	opaquePsoDesc.pRootSignature = m_RootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["baseVS"]->GetBufferPointer()),
		m_Shaders["baseVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["basePS"]->GetBufferPointer()),
		m_Shaders["basePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//opaquePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_BackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xmsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = m_DepthStencilFormat;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["base"])));

	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["baseWF"])));
	opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowMapPsoDesc = opaquePsoDesc;
	shadowMapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	shadowMapPsoDesc.NumRenderTargets = 0;
	shadowMapPsoDesc.RasterizerState.DepthBias = 30000;
	shadowMapPsoDesc.RasterizerState.DepthBiasClamp = 0.02f;
	shadowMapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	shadowMapPsoDesc.VS = {
		reinterpret_cast<BYTE*>(m_Shaders["shadowMapVS"]->GetBufferPointer()),
		m_Shaders["shadowMapVS"]->GetBufferSize()
	};
	shadowMapPsoDesc.PS = {
		reinterpret_cast<BYTE*>(m_Shaders["shadowMapPS"]->GetBufferPointer()),
		m_Shaders["shadowMapPS"]->GetBufferSize()
	};
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&shadowMapPsoDesc, IID_PPV_ARGS(&m_PSOs["shadowMap"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC cubeMapPsoDesc = opaquePsoDesc;
	cubeMapPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	cubeMapPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	cubeMapPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["cubeMapVS"]->GetBufferPointer()),
		m_Shaders["cubeMapVS"]->GetBufferSize()
	};
	cubeMapPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["cubeMapPS"]->GetBufferPointer()),
		m_Shaders["cubeMapPS"]->GetBufferSize()
	};
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&cubeMapPsoDesc, IID_PPV_ARGS(&m_PSOs["cubeMap"])));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;
	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	transparentPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	transparentPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["uiVS"]->GetBufferPointer()),
		m_Shaders["uiVS"]->GetBufferSize()
	};

	transparentPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["uiPS"]->GetBufferPointer()),
		m_Shaders["uiPS"]->GetBufferSize()
	};

	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&m_PSOs["ui"])));

	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.InputLayout = { m_CVertexInputLayout.data(), (UINT)m_CVertexInputLayout.size() };
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["drawElementVS"]->GetBufferPointer()),
		m_Shaders["drawElementVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["drawElementPS"]->GetBufferPointer()),
		m_Shaders["drawElementPS"]->GetBufferSize()
	};
	opaquePsoDesc.InputLayout = { m_CVertexInputLayout.data(),(UINT)m_CVertexInputLayout.size() };

	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["drawElement"])));

	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["drawLine"])));
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	opaquePsoDesc.BlendState.RenderTarget[0].BlendEnable = true;
	opaquePsoDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	opaquePsoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	opaquePsoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_DEST_ALPHA;
	opaquePsoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	opaquePsoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	opaquePsoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_DEST_ALPHA;
	opaquePsoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_MAX;
	opaquePsoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	opaquePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["planes"])));

	RENDERITEMMG->AddRenderSet("base");
	RENDERITEMMG->AddRenderSet("cubeMap");
	RENDERITEMMG->AddRenderSet("objectCoordinator");
	RENDERITEMMG->AddRenderSet("plane");
	RENDERITEMMG->AddRenderSet("line");
	RENDERITEMMG->AddRenderSet("dot");
	RENDERITEMMG->AddRenderSet("ui");
	RENDERITEMMG->AddRenderSet(cMesh::m_meshRenderName);
}

void D12D3Maaax::BuildTextures()
{
	wstring fileName;
	const wstring folderName = L"Texture/";
	m_TextureNames.push_back("ui.png");
	m_TextureNames.push_back("plane.png");
	m_TextureNames.push_back("line.png");
	m_TextureNames.push_back("dots.png");
	m_TextureNames.push_back("writing.png");
	m_TextureNames.push_back("push.png");
	m_TextureNames.push_back("CreateMesh.png");
	m_TextureNames.push_back("ice.dds");

	vector<string> otherUsesTextures;
	otherUsesTextures.push_back("cubeMap");
	otherUsesTextures.push_back("shadowMap");

	m_TextureHeap = make_unique<cTextureHeap>(m_D3dDevice.Get(), (UINT)m_TextureNames.size() + (UINT)otherUsesTextures.size());

	for (auto& it : m_TextureNames)
	{
		fileName.assign(it.begin(), it.end());
		m_TextureHeap->AddTexture(m_CommandQueue.Get(), it, folderName + fileName);
	}

	m_TextureHeap->AddCubeMapTexture(m_CommandQueue.Get(), "cubeMap", folderName + L"cubeMap.dds");
	BuildShadowMap();
}

void D12D3Maaax::BuildShadowMap()
{
	{
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = m_ClientWidth;
		texDesc.Height = m_ClientHeight;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear;
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		m_TextureHeap->AddNullTexture("shadowMap", DXGI_FORMAT_R24_UNORM_X8_TYPELESS, &texDesc, &optClear);
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_D3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_ShadowDsvHeap.GetAddressOf())));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.Texture2D.MipSlice = 0;
		m_D3dDevice->CreateDepthStencilView(m_TextureHeap->GetTexture("shadowMap").Get(), &dsvDesc,
			m_ShadowDsvHeap->GetCPUDescriptorHandleForHeapStart());
	}
}

void D12D3Maaax::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; i++)
	{
		m_FrameResources.push_back(make_unique<FrameResource>(m_D3dDevice.Get(),
			2, (UINT)m_Materials.size()));
	}
}

void D12D3Maaax::BuildMaterials()
{
	for (auto& it : m_TextureNames)
	{
		auto material = make_unique<Material>();
		material->name = it;
		material->matCBIndex = (int)m_Materials.size();
		material->diffuseMapIndex = m_TextureHeap->GetTextureIndex(it);
		material->diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		material->fresnel0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
		material->roughness = 0.25f;

		m_Materials[it] = move(material);
	}
}

void D12D3Maaax::BuildObjects()
{
	cOperation::OperationsBaseSetup();
	m_operator = make_unique<cOperator>();
	m_operator->Build();

	m_cubeMapRender = RENDERITEMMG->AddRenderItem("cubeMap");
	m_cubeMapRender->SetGeometry(m_cubeMapSphere, m_cubeMapSphere->name);
	m_cubeMapRenderInstance = m_cubeMapRender->GetRenderIsntance();
	m_cubeMapRenderInstance->m_isRenderOK = true;
	XMStoreFloat4x4(&m_cubeMapRenderInstance->instanceData.World, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f));

	OBJCOORD->Build();
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> D12D3Maaax::GetStaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp,
		shadow
	};
}

