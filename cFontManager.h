#pragma once
#define FONTMANAGER cFontManager::Get()

struct RenderFont
{
	bool isRender = true;
	SpriteFont* font=nullptr;
	wstring printString;
	float rotation=0;
	DirectX::SimpleMath::Vector3 pos={0, 0, 0};
	DirectX::SimpleMath::Vector3 scale={1, 1, 1};
	DirectX::SimpleMath::Vector4 color = { 0,0,0,0 };
};

class cFontManager
{
public:
	~cFontManager();

	static cFontManager* Get()
	{
		if (instance == nullptr)
		{
			instance = new cFontManager;
		}

		return instance;
	}

	void Build(ID3D12Device* device, ID3D12CommandQueue* queue);
	void AddFont(ID3D12Device * device, ID3D12CommandQueue * queue, const string& fontName ,const wstring& fileName);
	shared_ptr<RenderFont> GetFont(string fontName);

	void Resize(UINT clientWidth, UINT clientHeight);
	void Render(ID3D12GraphicsCommandList* cmdList, ID3D12CommandQueue* cmdQueue);

private:
	cFontManager();

	enum Descriptors
	{
		BaseFont,
		Count,
	};

private:
	static cFontManager* instance;

	unique_ptr<GraphicsMemory> m_graphicsMemory = nullptr;
	unique_ptr<DescriptorHeap> m_resourceDescriptors = nullptr;
	unique_ptr<SpriteBatch> m_spriteBatch = nullptr;
	unordered_map<string, unique_ptr<SpriteFont>> m_fonts;
	list<shared_ptr<RenderFont>> m_renderFonts;
};

