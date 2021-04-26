#include "D3dClass.h"

bool D3dClass::Init(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, 
					bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT hr;
	IDXGIFactory* factory;
	IDXGIAdapter* Adapter;
	IDXGIOutput* AdapterOutput;
	int numMode, i, Numerator, Denominator, stringLength;
	DXGI_MODE_DESC* DisplayModeList;
	DXGI_ADAPTER_DESC AdapterDesc;
	int Error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D11Texture2D* BackBuffer;
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	D3D11_RASTERIZER_DESC RasterDesc;
	D3D11_VIEWPORT ViewPort;
	float FieldOfView, ScreenAspect;

	// vsync(수직동기화) 설정을 저장
	// 수직동기화 = 렌더링을 모니터 주사율에 맞출 것인지, 가능한 한 빠르게 다시 그릴것인지 지정.
	m_vsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리 생성.
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(hr))
	{
		return false;
	}

	// 팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스에 대한 어댑터를 생성
	hr = factory->EnumAdapters(0, &Adapter);
	if (FAILED(hr))
	{
		return false;
	}

	// 출력(모니터)에 대한 첫번째 어댑터 나열
	hr = Adapter->EnumOutputs(0, &AdapterOutput);
	if (FAILED(hr))
	{
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 모드의 개수를 가져옴.
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, (UINT*)&numMode, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트 생성
	DisplayModeList = new DXGI_MODE_DESC[numMode];
	if (!DisplayModeList)
	{
		return false;
	}

	// 디스플레이 모드에 대한 리스트 구조를 채움.
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, (UINT*)&numMode, DisplayModeList);
	if (FAILED(hr))
	{
		return false;
	}

	// 이제 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드를 검색.
	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자값 저장.
	for (int i = 0; i < numMode; i++)
	{
		if (DisplayModeList[i].Width == ScreenWidth)
		{
			if (DisplayModeList[i].Height == ScreenHeight)
			{
				Numerator = DisplayModeList[i].RefreshRate.Numerator;
				Denominator = DisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 어댑터(그래픽카드)와 Description을 가져옴.
	hr = Adapter->GetDesc(&AdapterDesc);
	if (FAILED(hr))
	{
		return false;
	}

	// 현재 그래픽카드의 메모리 용량을 메가바이트 단위로 저장.
	m_VideoCardMemory = (AdapterDesc.DedicatedVideoMemory / 1024 / 1024);


	return true;
}



D3dClass::D3dClass()
{
	m_SwapChain = nullptr;
	m_pd3dDevice = nullptr;
	m_pDeviceContext = nullptr;
	m_RenderTargetView = nullptr;
	m_DepthStencilBuffer = nullptr;
	m_DepthStencilState = nullptr;
	m_DepthStencilView = nullptr;
	m_RasterizerState = nullptr;
}
