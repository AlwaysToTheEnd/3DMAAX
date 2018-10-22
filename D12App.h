#pragma once
#define DELTATIME D12App::GetDeltaTime()
#define CAMERA D12App::GetCamera()

class D12App
{
protected:
	D12App(HINSTANCE hInstance);
	D12App(const D12App& rhs) = delete;
	D12App& operator=(const D12App& rhs) = delete;
	virtual ~D12App();

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();

public:
	static D12App*	GetApp();
	static double	GetDeltaTime() { return m_DeltaTime; }
	static cCamera& GetCamera() { return m_Camera; }
	static UINT		GetRandom() { return m_random(); }

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;
	float     AspectRatio()const;

	bool Get4xMsaaState()const;
	void Set4xMsaaState(bool value);

	int Run();

	virtual bool Initialize();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObject();
	void CreateSwapChain();

	void FlushCommandQueue();
	void CalculateFrameStats();

	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);


protected:
	static D12App*	m_Instance;
	static double	m_DeltaTime;
	static mt19937	m_random;
	static cCamera	m_Camera;

	HINSTANCE		mhInstance = nullptr;
	HWND			mhMainWnd = nullptr;
	bool			mAppPaused = false;
	bool			mMinimized = false;
	bool			mMaximized = false;
	bool			mResizing = false;
	bool			mFullscreenState = false;

	bool			m_4xMsaaState = false;
	UINT			m_4xmsaaQuality = 0;

	D3D12_VIEWPORT	m_ScreenViewport;
	D3D12_RECT		m_ScissorRect;
	wstring			m_MainWndCaption = L"DX12 CGH 3DMAAAAX";
	D3D_DRIVER_TYPE	m_D3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	GameTimer		m_Timer;

	ComPtr<IDXGIFactory4>	m_DxgiFactory;
	ComPtr<IDXGISwapChain>	m_SwapChain;
	DXGI_FORMAT				m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~1 
	DXGI_FORMAT				m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int						m_ClientWidth = 1000;
	int						m_ClientHeight = 700;

	ComPtr<ID3D12Device>	m_D3dDevice;

	ComPtr<ID3D12Fence>		m_Fence;
	UINT64					m_CurrentFence = 0;

	ComPtr<ID3D12CommandQueue>		m_CommandQueue;
	ComPtr<ID3D12CommandAllocator>	m_DirectCmdListAlloc;
	ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	static const int				SwapChainBufferCount = 2;
	int								m_CurrBackBuffer = 0;
	ComPtr<ID3D12Resource>			m_SwapChainBuffer[SwapChainBufferCount];
	ComPtr<ID3D12Resource>			m_DepthStencilBuffer;

	ComPtr<ID3D12DescriptorHeap>	m_RTVHeap;
	ComPtr<ID3D12DescriptorHeap>	m_DSVHeap;
	UINT							m_RTVDescriptorSize = 0;
	UINT							m_DSVDescriptorSize = 0;
	UINT							m_CBV_SRV_UAV_DescriptorSize = 0;
	
};
