#pragma once

class D12TextureApp :public D12App
{
public:
	D12TextureApp(HINSTANCE hInstance);
	D12TextureApp(const D12TextureApp& rhs) = delete;
	D12TextureApp& operator=(const D12TextureApp& rhs) = delete;
	virtual ~D12TextureApp();

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
	vector<unique_ptr<FrameResource>> m_FrameResources;
	FrameResource* m_CurrFrameResource = nullptr;
	int m_CurrFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

	unique_ptr<cTextureHeap> m_TextureHeap;
	unordered_map<string, unique_ptr<Material>> m_Materials;
	unordered_map<string, ComPtr<ID3DBlob>> m_Shaders;
	unordered_map<string, unique_ptr<MeshGeometry>> m_Geometries;
	unordered_map<string, ComPtr<ID3D12PipelineState>> m_PSOs;

	vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	PassConstants m_MainPassCB;

	XMFLOAT4X4 m_Proj = MathHelper::Identity4x4();
	mt19937 m_random;

private:
	unique_ptr<cObject> m_sphere = nullptr;
	RenderFont* m_font = nullptr;
	cUIObject m_UIObject;
};