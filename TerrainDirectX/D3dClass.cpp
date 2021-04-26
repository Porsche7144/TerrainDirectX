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

	// vsync(��������ȭ) ������ ����
	// ��������ȭ = �������� ����� �ֻ����� ���� ������, ������ �� ������ �ٽ� �׸������� ����.
	m_vsync_enabled = vsync;

	// DirectX �׷��� �������̽� ���丮 ����.
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(hr))
	{
		return false;
	}

	// ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� ����͸� ����
	hr = factory->EnumAdapters(0, &Adapter);
	if (FAILED(hr))
	{
		return false;
	}

	// ���(�����)�� ���� ù��° ����� ����
	hr = Adapter->EnumOutputs(0, &AdapterOutput);
	if (FAILED(hr))
	{
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ����� ������ ������.
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, (UINT*)&numMode, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ ����
	DisplayModeList = new DXGI_MODE_DESC[numMode];
	if (!DisplayModeList)
	{
		return false;
	}

	// ���÷��� ��忡 ���� ����Ʈ ������ ä��.
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, (UINT*)&numMode, DisplayModeList);
	if (FAILED(hr))
	{
		return false;
	}

	// ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 �˻�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���ڰ� ����.
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

	// �����(�׷���ī��)�� Description�� ������.
	hr = Adapter->GetDesc(&AdapterDesc);
	if (FAILED(hr))
	{
		return false;
	}

	// ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ ����.
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
