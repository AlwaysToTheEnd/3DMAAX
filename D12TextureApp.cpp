#include "stdafx.h"

D12TextureApp::D12TextureApp(HINSTANCE hInstance)
	:D12App(hInstance)
	, m_random(10)
{

}

D12TextureApp::~D12TextureApp()
{
	delete RENDERITEMMG;
	delete FONTMANAGER;

	if (m_D3dDevice != nullptr)
		FlushCommandQueue();
}

bool D12TextureApp::Initialize()
{
	if (!D12App::Initialize())
		return false;

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
	FONTMANAGER->Build(m_D3dDevice.Get(), m_CommandQueue.Get());

	//m_font = FONTMANAGER->GetFont("baseFont");
	//m_font->printString = L"sdalkfjaklsdf";
	//m_font->color = Colors::White;
	//m_font->pos = { 0,0,0 };
	for (auto& it : m_Geometries)
	{
		it.second->DisPosUploaders();
	}
	
	OnResize();

	return true;

}

void D12TextureApp::OnResize()
{
	D12App::OnResize();

	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_Proj, P);

	FONTMANAGER->Resize(m_ClientWidth, m_ClientHeight);
}

void D12TextureApp::Update()
{
	m_Camera.Update();

	m_CurrFrameResourceIndex = (m_CurrFrameResourceIndex + 1) % gNumFrameResources;
	m_CurrFrameResource = m_FrameResources[m_CurrFrameResourceIndex].get();

	if (m_CurrFrameResource->fence != 0 && m_Fence->GetCompletedValue() < m_CurrFrameResource->fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_CurrFrameResource->fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}


	//m_Mirror->Update(nullptr);

	m_UIObject.Update(XMMatrixIdentity());

	RENDERITEMMG->Update(m_CurrFrameResource);
	UpdateMaterialCBs();
	UpdateMainPassCB();
}

void D12TextureApp::Draw()
{
	auto cmdListAlloc = m_CurrFrameResource->cmdListAlloc.Get();

	ThrowIfFailed(cmdListAlloc->Reset());

	ThrowIfFailed(m_CommandList->Reset(cmdListAlloc,
		m_PSOs["opaque"].Get()));

	m_CommandList->RSSetViewports(1, &m_ScreenViewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	m_CommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	//m_CommandList->ClearRenderTargetView(ShadowMapView(), Colors::White, 0, nullptr);
	m_CommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_CommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	/////////////////////////////

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_TextureHeap->GetHeap() };
	m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	auto passCBAddress = m_CurrFrameResource->passCB->Resource()->GetGPUVirtualAddress();
	m_CommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);
	RENDERITEMMG->Render(m_CommandList.Get(), m_CurrFrameResource, "opaque");

	//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap.Get(),
	//	D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_ShadowMap.Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	////////////////
	m_CommandList->OMSetStencilRef(1);
	m_CommandList->SetPipelineState(m_PSOs["markStencilMirrors"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), m_CurrFrameResource, "Mirror");

	m_CommandList->SetPipelineState(m_PSOs["drawStencilReflections"].Get());
	passCBAddress += d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	m_CommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);
	RENDERITEMMG->Render(m_CommandList.Get(), m_CurrFrameResource, "reflectionObject");

	passCBAddress -= d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
	m_CommandList->OMSetStencilRef(0);

	m_CommandList->SetPipelineState(m_PSOs["transparent"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), m_CurrFrameResource, "Mirror");

	passCBAddress += d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants))*2;
	m_CommandList->SetGraphicsRootConstantBufferView(2, passCBAddress);

	m_CommandList->SetPipelineState(m_PSOs["UI"].Get());
	RENDERITEMMG->Render(m_CommandList.Get(), m_CurrFrameResource, "UI");

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


void D12TextureApp::UpdateMaterialCBs()
{
	auto currMaterialCB = m_CurrFrameResource->materialCB.get();

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
			XMStoreFloat4x4(&matConstants.matTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->matCBIndex, matConstants);

			mat->numFramesDirty--;
		}
	}
}

void D12TextureApp::UpdateMainPassCB()
{
	XMMATRIX view = XMLoadFloat4x4(m_Camera.GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&m_Proj);

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

	auto currPassCB = m_CurrFrameResource->passCB.get();
	currPassCB->CopyData(0, m_MainPassCB);

	XMMATRIX R = XMLoadFloat4x4(&m_Mirror->GetRenderItem()->world);

	// Reflect the lighting.
	for (int i = 0; i < 3; ++i)
	{
		XMVECTOR lightDir = XMLoadFloat3(&m_MainPassCB.Lights[i].direction);
		XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
		XMStoreFloat3(&m_MainPassCB.Lights[i].direction, reflectedLightDir);
	}

	currPassCB->CopyData(1, m_MainPassCB);


	m_MainPassCB.view = MathHelper::Identity4x4();
	m_MainPassCB.invView = MathHelper::Identity4x4();
	proj = XMMatrixOrthographicOffCenterLH(0, m_ClientWidth, m_ClientHeight, 0, -1, 1);
	XMStoreFloat4x4(&m_MainPassCB.proj, XMMatrixTranspose(proj));
	m_MainPassCB.invProj = MathHelper::Identity4x4();
	m_MainPassCB.viewProj = MathHelper::Identity4x4();
	m_MainPassCB.invViewProj = MathHelper::Identity4x4();

	currPassCB->CopyData(2, m_MainPassCB);
}


void D12TextureApp::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParam[4];
	slotRootParam[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParam[1].InitAsConstantBufferView(0);
	slotRootParam[2].InitAsConstantBufferView(1);
	slotRootParam[3].InitAsConstantBufferView(2);

	auto staticSamplers = GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootDesc;
	rootDesc.Init(4, slotRootParam, (UINT)staticSamplers.size(),
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

void D12TextureApp::BuildShadersAndInputLayout()
{
	m_Shaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_0");
	m_Shaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_0");

	m_Shaders["uiVS"] = d3dUtil::CompileShader(L"Shaders\\UIShader.hlsl", nullptr, "VS", "vs_5_0");
	m_Shaders["uiPS"] = d3dUtil::CompileShader(L"Shaders\\UIShader.hlsl", nullptr, "PS", "ps_5_0");

	m_InputLayout =
	{
		{ "POSITION" ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void D12TextureApp::BuildGeometry()
{
	vector<Vertex> vertices;
	vector<UINT> indices;

	SubMeshGeometry subMesh;
	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = indices.size();

	///////////////////////

	auto sphereGeo = make_unique<MeshGeometry>();
	sphereGeo->name = "sphere";

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.15f, 10, 10);
	vertices.clear();
	vertices.resize(sphere.Vertices.size());

	const UINT sphereDataSize = sizeof(Vertex)*vertices.size();
	const UINT sphereIndexSize = sizeof(UINT32)*sphere.Indices32.size();

	for (int i = 0; i < sphere.Vertices.size(); i++)
	{
		vertices[i].pos = sphere.Vertices[i].Position;
		vertices[i].normal = sphere.Vertices[i].Normal;
		vertices[i].uv = sphere.Vertices[i].TexC;
	}

	sphereGeo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_D3dDevice.Get(), m_CommandList.Get(),
		vertices.data(), sphereDataSize, sphereGeo->vertexUploadBuffer);

	sphereGeo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(m_D3dDevice.Get(), m_CommandList.Get(),
		sphere.Indices32.data(), sphereIndexSize, sphereGeo->indexUploadBuffer);

	sphereGeo->indexFormat = DXGI_FORMAT_R32_UINT;
	sphereGeo->indexBufferByteSize = sphereIndexSize;
	sphereGeo->vertexBufferByteSize = sphereDataSize;
	sphereGeo->vertexByteStride = sizeof(Vertex);

	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = sphere.Indices32.size();
	sphereGeo->DrawArgs["sphere"] = subMesh;
	m_Geometries[sphereGeo->name] = move(sphereGeo);

	//////////////////////

	//////////////
	auto cubeGeo = make_unique<MeshGeometry>();
	cubeGeo->name = "cube";
	GeometryGenerator::MeshData cube = geoGen.CreateBox(1, 1, 1, 1);
	vertices.clear();
	vertices.resize(cube.Vertices.size());

	const UINT cubeGeoDataSize = sizeof(Vertex)*vertices.size();
	const UINT cubeGeoIndexSize = sizeof(UINT16)*cube.Indices32.size();

	for (int i = 0; i < cube.Vertices.size(); i++)
	{
		vertices[i].pos = cube.Vertices[i].Position;
		vertices[i].normal = cube.Vertices[i].Normal;
		vertices[i].uv = cube.Vertices[i].TexC;
	}

	cubeGeo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_D3dDevice.Get(), m_CommandList.Get(),
		vertices.data(), cubeGeoDataSize, cubeGeo->vertexUploadBuffer);

	cubeGeo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(m_D3dDevice.Get(), m_CommandList.Get(),
		cube.GetIndices16().data(), cubeGeoIndexSize, cubeGeo->indexUploadBuffer);

	cubeGeo->indexFormat = DXGI_FORMAT_R16_UINT;
	cubeGeo->indexBufferByteSize = cubeGeoIndexSize;
	cubeGeo->vertexBufferByteSize = cubeGeoDataSize;
	cubeGeo->vertexByteStride = sizeof(Vertex);

	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = cube.Indices32.size();
	cubeGeo->DrawArgs["cube"] = subMesh;
	m_Geometries[cubeGeo->name] = move(cubeGeo);

	///////////
	auto mirrorGeo = make_unique<MeshGeometry>();
	mirrorGeo->name = "Mirror";
	vertices.resize(4);

	vertices[0] = { {0,0,0},{0,0,-1},{0,1} };
	vertices[1] = { {30,0,0},{0,0,-1},{1,1} };
	vertices[2] = { {30,30,0},{0,0,-1},{1,0} };
	vertices[3] = { {0,30,0},{0,0,-1},{0,0} };

	vector<UINT16> Indices;
	Indices.push_back(0);
	Indices.push_back(2);
	Indices.push_back(1);
	Indices.push_back(0);
	Indices.push_back(3);
	Indices.push_back(2);

	const UINT mirrorGeoDataSize = sizeof(Vertex)*vertices.size();
	const UINT mirrorGeoIndexSize = sizeof(UINT16)*Indices.size();

	mirrorGeo->vertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_D3dDevice.Get(), m_CommandList.Get(),
		vertices.data(), mirrorGeoDataSize, mirrorGeo->vertexUploadBuffer);

	mirrorGeo->indexBufferGPU = d3dUtil::CreateDefaultBuffer(m_D3dDevice.Get(), m_CommandList.Get(),
		Indices.data(), mirrorGeoIndexSize, mirrorGeo->indexUploadBuffer);

	mirrorGeo->indexFormat = DXGI_FORMAT_R16_UINT;
	mirrorGeo->indexBufferByteSize = mirrorGeoIndexSize;
	mirrorGeo->vertexBufferByteSize = mirrorGeoDataSize;
	mirrorGeo->vertexByteStride = sizeof(Vertex);

	subMesh.baseVertexLocation = 0;
	subMesh.startIndexLocation = 0;
	subMesh.indexCount = Indices.size();
	mirrorGeo->DrawArgs["Mirror"] = subMesh;
	m_Geometries[mirrorGeo->name] = move(mirrorGeo);

	cUIObject::UIMeshSetUp(m_D3dDevice.Get(), m_CommandList.Get());
}

void D12TextureApp::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	opaquePsoDesc.pRootSignature = m_RootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["standardVS"]->GetBufferPointer()),
		m_Shaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_Shaders["opaquePS"]->GetBufferPointer()),
		m_Shaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_BackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_4xMsaaState ? (m_4xmsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = m_DepthStencilFormat;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&m_PSOs["opaque"])));


	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;
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

	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&m_PSOs["transparent"])));

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

	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&m_PSOs["UI"])));

	CD3DX12_BLEND_DESC mirrorBlendState(D3D12_DEFAULT);
	mirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;

	D3D12_DEPTH_STENCIL_DESC mirrorDSS;
	mirrorDSS.DepthEnable = true;
	mirrorDSS.DepthEnable = true;
	mirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	mirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	mirrorDSS.StencilEnable = true;
	mirrorDSS.StencilReadMask = 0xff;
	mirrorDSS.StencilWriteMask = 0xff;

	mirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	mirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	mirrorDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	mirrorDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	mirrorDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC markMirrorsPsoDesc = opaquePsoDesc;
	markMirrorsPsoDesc.BlendState = mirrorBlendState;
	markMirrorsPsoDesc.DepthStencilState = mirrorDSS;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&markMirrorsPsoDesc, IID_PPV_ARGS(&m_PSOs["markStencilMirrors"])));

	D3D12_DEPTH_STENCIL_DESC reflectionsDSS;
	reflectionsDSS.DepthEnable = true;
	reflectionsDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	reflectionsDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	reflectionsDSS.StencilEnable = true;
	reflectionsDSS.StencilReadMask = 0xff;
	reflectionsDSS.StencilWriteMask = 0xff;

	reflectionsDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	reflectionsDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	reflectionsDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC drawReflectionsPsoDesc = opaquePsoDesc;
	drawReflectionsPsoDesc.DepthStencilState = reflectionsDSS;
	drawReflectionsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	drawReflectionsPsoDesc.RasterizerState.FrontCounterClockwise = true;
	ThrowIfFailed(m_D3dDevice->CreateGraphicsPipelineState(&drawReflectionsPsoDesc, IID_PPV_ARGS(&m_PSOs["drawStencilReflections"])));

	RENDERITEMMG->AddRenderSet("opaque");
	RENDERITEMMG->AddRenderSet("UI");
	RENDERITEMMG->AddRenderSet("Mirror");
	RENDERITEMMG->AddRenderSet("reflectionObject");
	cMirror::SetReflectionRenderPipeLine("reflectionObject");
}

void D12TextureApp::BuildTextures()
{
	m_TextureHeap = make_unique<cTextureHeap>(m_D3dDevice.Get(), 15);
	m_TextureHeap->AddTexture(m_CommandQueue.Get(), "ice", L"Texture/ice.dds");
}

void D12TextureApp::BuildFrameResources()
{
	for (int i = 0; i < gNumFrameResources; i++)
	{
		m_FrameResources.push_back(make_unique<FrameResource>(m_D3dDevice.Get(),
			3, RENDERITEMMG->GetNumRenderItems(), (UINT)m_Materials.size()));
	}
}


void D12TextureApp::BuildMaterials()
{
	auto wirefence = make_unique<Material>();
	wirefence->name = "wirefence";
	wirefence->matCBIndex = m_Materials.size();
	wirefence->diffuseSrvHeapIndex = 0;
	wirefence->diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	wirefence->fresnel0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	wirefence->roughness = 0.25f;

	m_Materials["wirefence"] = move(wirefence);

	auto icemirror = std::make_unique<Material>();
	icemirror->name = "icemirror";
	icemirror->matCBIndex = m_Materials.size();
	icemirror->diffuseSrvHeapIndex = 2;
	icemirror->diffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f);
	icemirror->fresnel0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	icemirror->roughness = 0.5f;

	m_Materials["icemirror"] = std::move(icemirror);
}

void D12TextureApp::BuildObjects()
{
	m_UIObject.Build("UI");
	m_UIObject.SetTexture(m_TextureHeap->GetTexture("ice"));
	m_UIObject.SetMaterial(m_Materials["wirefence"].get());
	m_UIObject.SetSize({ 300,300 });
	m_UIObject.SetPos({ 0,0,0 });

	m_Mirror = make_unique<cMirror>();
	m_Mirror->Build("Mirror", m_Geometries["Mirror"].get(),
		m_Materials["icemirror"].get(), m_TextureHeap->GetTexture("ice"), "Mirror");
	m_Mirror->SetPos({ 5,15,15 });
	m_Mirror->SetRotX(-XM_PIDIV4);

	m_sphere= make_unique<cObject>();
	m_sphere->Build("sphere", m_Geometries["sphere"].get(),
		m_Materials["wirefence"].get(), m_TextureHeap->GetTexture("ice"));
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> D12TextureApp::GetStaticSamplers()
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

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}

