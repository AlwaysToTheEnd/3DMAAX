#pragma once

class D12D3Maaax :public D12App
{
public:
	D12D3Maaax(HINSTANCE hInstance);
	D12D3Maaax(const D12D3Maaax& rhs) = delete;
	D12D3Maaax& operator=(const D12D3Maaax& rhs) = delete;
	virtual ~D12D3Maaax();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update() override;
	virtual void Draw() override;

	void UpdateMaterialCBs();
	void UpdateMainPassCB();

	void BuildTextures();
	void BuildMaterials();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildGeometry();
	void BuildPSOs();
	void BuildObjects();
	void BuildFrameResources();

	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

private:
	vector<unique_ptr<FrameResource>>	m_FrameResources;
	FrameResource*						m_CurrFrameResource = nullptr;
	int									m_CurrFrameResourceIndex = 0;

	unordered_map<string, ComPtr<ID3D12PipelineState>> m_PSOs;
	unordered_map<string, unique_ptr<Material>>	m_Materials;
	unique_ptr<cTextureHeap>					m_TextureHeap = nullptr;
	vector<string>								m_TextureNames;

	ComPtr<ID3D12RootSignature>				m_RootSignature = nullptr;
	unordered_map<string, ComPtr<ID3DBlob>>	m_Shaders;
	vector<D3D12_INPUT_ELEMENT_DESC>		m_NTVertexInputLayout;
	vector<D3D12_INPUT_ELEMENT_DESC>		m_CVertexInputLayout;

	PassConstants	m_MainPassCB;
	XMFLOAT4X4		m_3DProj = MathHelper::Identity4x4();
	XMFLOAT4X4		m_2DProj = MathHelper::Identity4x4();

private:
	unique_ptr<cOperator>	m_operator =nullptr;
	bool					m_isBaseWireFrameMode = false;
};