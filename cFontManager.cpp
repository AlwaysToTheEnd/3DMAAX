#include "stdafx.h"

cFontManager* cFontManager::instance = nullptr;

void cFontManager::Build(ID3D12Device * device, ID3D12CommandQueue * queue)
{
	m_graphicsMemory = std::make_unique<GraphicsMemory>(device);

	m_resourceDescriptors = std::make_unique<DescriptorHeap>(device,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		Descriptors::Count);

	ResourceUploadBatch resourceUpload(device);
	resourceUpload.Begin();

	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
	SpriteBatchPipelineStateDescription pd(rtState);
	m_spriteBatch = std::make_unique<SpriteBatch>(device, resourceUpload, pd);

	m_fonts["baseFont"] = std::make_unique<SpriteFont>(device, resourceUpload,
		L"font/aBlackB.spritefont",
		m_resourceDescriptors->GetCpuHandle(Descriptors::BaseFont),
		m_resourceDescriptors->GetGpuHandle(Descriptors::BaseFont));

	m_fonts["baseFont_K"] = std::make_unique<SpriteFont>(device, resourceUpload,
		L"font/aBlackB_K.spritefont",
		m_resourceDescriptors->GetCpuHandle(Descriptors::BaseFont_K),
		m_resourceDescriptors->GetGpuHandle(Descriptors::BaseFont_K));

	auto uploadResourcesFinished = resourceUpload.End(queue);

	uploadResourcesFinished.wait();
}

void cFontManager::AddFont(ID3D12Device * device, ID3D12CommandQueue * queue, const string & fontName, const wstring & fileName)
{
	/*ResourceUploadBatch resourceUpload(device);
	resourceUpload.Begin();

	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
	SpriteBatchPipelineStateDescription pd(rtState);
	m_spriteBatch = std::make_unique<SpriteBatch>(device, resourceUpload, pd);

	m_fonts[fontName] = std::make_unique<SpriteFont>(device, resourceUpload,
		fileName.c_str(),
		m_resourceDescriptors->GetCpuHandle(Descriptors::BaseFont),
		m_resourceDescriptors->GetGpuHandle(Descriptors::BaseFont));

	auto uploadResourcesFinished = resourceUpload.End(queue);

	uploadResourcesFinished.wait();*/
}

void cFontManager::Resize(UINT clientWidth, UINT clientHeight)
{
	if (m_spriteBatch)
	{
		D3D12_VIEWPORT viewPort = { 0.0f,0.0f,
			static_cast<float>(clientWidth),static_cast<float>(clientHeight),
			D3D12_MIN_DEPTH,D3D12_MAX_DEPTH };

		m_spriteBatch->SetViewport(viewPort);
	}
}

shared_ptr<RenderFont> cFontManager::GetFont(string fontName)
{
	auto it = m_fonts.find(fontName);
	if (it != m_fonts.end())
	{
		m_renderFonts.push_back(make_shared<RenderFont>());
		m_renderFonts.back()->font = it->second.get();
		return m_renderFonts.back();
	}
	else
	{
		return nullptr;
	}
}

void cFontManager::Render(ID3D12GraphicsCommandList * cmdList)
{
	ID3D12DescriptorHeap* heaps[] = { m_resourceDescriptors->Heap() };
	cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

	m_spriteBatch->Begin(cmdList);

	DirectX::SimpleMath::Vector2 origin = {0,0};

	for (auto it = m_renderFonts.begin(); it != m_renderFonts.end();)
	{
		if (it->use_count() == 1)
		{
			it = m_renderFonts.erase(it);
		}
		else
		{
			if ((*it)->isRender)
			{
				SpriteFont* font = (*it)->font;
				font->DrawString(m_spriteBatch.get(), (*it)->printString.c_str(), (*it)->pos,
					(*it)->color, (*it)->rotation, origin, (*it)->scale);
			}

			it++;
		}
	}

	m_spriteBatch->End();
}

void cFontManager::Commit(ID3D12CommandQueue * queue)
{
	m_graphicsMemory->Commit(queue);
}

cFontManager::cFontManager()
{
}


cFontManager::~cFontManager()
{
}
