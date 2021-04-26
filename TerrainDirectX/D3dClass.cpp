#include "D3dClass.h"

// ����̽��� ���ؽ�Ʈ�� �����͸� �������� �Լ�
ID3D11Device* D3dClass::GetDevice()
{
	return m_pd3dDevice.Get();
}

ID3D11DeviceContext* D3dClass::GetDeviceContext()
{
	return m_pDeviceContext.Get();
}

// ����, ����, ���� ��������� ��ȯ�ϴ� �Լ�
void D3dClass::GetProjectionMatrix(Matrix ProjMatrix)
{
	ProjMatrix = m_matProjection;
	return;
}

void D3dClass::GetWorldMatrix(Matrix matWorld)
{
	matWorld = m_matWorld;
	return;
}

void D3dClass::GetOrthoMatrix(Matrix OrthoMatrix)
{
	OrthoMatrix = m_matOrthoMatrix;
	return;
}

// �׷���ī�� �̸��� ��밡���� �޸� �� ��ȯ(������)
void D3dClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_VideoCardDescription);
	memory = m_VideoCardMemory;
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////


Matrix D3dClass::CreateProjectionMatrix(float fFov, float Aspect, float fNear, float fFar)
{
	m_matProjection = Matrix::CreatePerspectiveFieldOfView(fFov, Aspect, fNear, fFar);

	return m_matProjection;
}

Matrix D3dClass::CreateOrthoMatrix(float Width, float Height, float fNear, float fFar)
{
	m_matOrthoMatrix = Matrix::CreateOrthographic(Width, Height, fNear, fFar);

	return m_matOrthoMatrix;
}

bool D3dClass::Init(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, 
					bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT hr;
	IDXGIFactory* Factory;
	IDXGIAdapter* Adapter;
	IDXGIOutput* AdapterOutput;
	unsigned int numMode, i, Numerator, Denominator;
	size_t stringLength;
	DXGI_MODE_DESC* DisplayModeList;
	DXGI_ADAPTER_DESC AdapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	D3D_FEATURE_LEVEL FeatureLevel;
	ID3D11Texture2D* BackBufferPtr;
	D3D11_TEXTURE2D_DESC DepthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc;
	D3D11_RASTERIZER_DESC RasterDesc;
	D3D11_VIEWPORT Viewport;
	float FieldOfView, ScreenAspect;


	// vsync(��������ȭ) ���� ����.
	// ��������ȭ = �������� ����� �ֻ����� ���� ������, ������ �� ������ �ٽ� �׸�������
	m_vsync_enabled = vsync;

	// DirectX �׷��� �������̽� ���丮 ����
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);
	if (FAILED(hr))
	{
		return false;
	}

	// ���丮 ��ü�� ����� ù��° �׷��� ī�� �������̽��� ���� ����� ����
	hr = Factory->EnumAdapters(0, &Adapter);
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

	// DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ��� ������ ����
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numMode, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	// ������ ��� ����Ϳ� �׷��� ī�� ������ ������ ����Ʈ ����
	DisplayModeList = new DXGI_MODE_DESC[numMode];
	if (!DisplayModeList)
	{
		return false;
	}

	// ���÷��� ��忡 ���� ����Ʈ ������ ä��
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numMode, DisplayModeList);
	if (FAILED(hr))
	{
		return false;
	}

	// ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��� �˻�.
	// ������ ���� ã���� ������� ���ΰ�ħ ���� ����.
	for (i = 0; i < numMode; i++)
	{
		if (DisplayModeList[i].Width == (unsigned int)ScreenWidth)
		{
			if (DisplayModeList[i].Height == (unsigned int)ScreenHeight)
			{
				Numerator = DisplayModeList[i].RefreshRate.Numerator;
				Denominator = DisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// �����(�׷���ī��)�� ����ü�� ������
	hr = Adapter->GetDesc(&AdapterDesc);
	if (FAILED(hr))
	{
		return false;
	}

	// ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ ����
	m_VideoCardMemory = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �׷���ī���� �̸��� char ���ڿ� �迭�� ����
	error = wcstombs_s(&stringLength, m_VideoCardDescription, 128, AdapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// ���÷��� ��� ����Ʈ�� �Ҵ� ����
	delete[] DisplayModeList;
	DisplayModeList = 0;

	// ��� ����� �Ҵ� ����
	AdapterOutput->Release();
	AdapterOutput = 0;

	// ����� �Ҵ� ����
	Adapter->Release();
	Adapter = 0;

	// ���丮 �Ҵ� ����
	Factory->Release();
	Factory = 0;


	// ����ü�� = ������ �� �� ����ۿ� ����Ʈ���� ��ü�ϱ� ���� ���.
	// ����ü�� ����ü �ʱ�ȭ
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// �ϳ��� ����� ����ϵ���
	SwapChainDesc.BufferCount = 1;

	// ����� �ʺ�/���� ����
	SwapChainDesc.BufferDesc.Width = ScreenWidth;
	SwapChainDesc.BufferDesc.Height = ScreenHeight;

	// ����۷� �����̽� ����
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ����� ���ΰ�ħ ���� ����
	if (m_vsync_enabled)
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = Numerator;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = Denominator;
	}
	else
	{
		SwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ������� �뵵 ����
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� �̷���� ������ �ڵ� ����
	SwapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø� Off
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;

	// ������ ��� �Ǵ� Ǯ��ũ�� ��� ����
	if (fullscreen)
	{
		SwapChainDesc.Windowed = false;
	}
	else
	{
		SwapChainDesc.Windowed = true;
	}

	// ��ĵ������ ���İ� ��ĵ���̴��� �������� ����(UNSPECIFIED)���� ����
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ���� ������� ������ ��������
	SwapChainDesc.SwapEffect - DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��� ��� ����.
	SwapChainDesc.Flags = 0;
	
	// ���ķ����� DirectX 11�� ����
	FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// ����ü��, Direct3D ����̽�, ���ؽ�Ʈ ����
	// D3D_DRIVER_TYPE_HARDWARE�� D3D_DRIVER_TYPE_REFERENCE �� �ٲپ� �����ϸ�
	// �׷���ī�尡 �ƴ� CPU���� ������ ó��. (�� 1/1000 ������ ����.)
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevel, 1, D3D11_SDK_VERSION, 
		&SwapChainDesc,	m_SwapChain.GetAddressOf(), m_pd3dDevice.GetAddressOf(), NULL, m_pDeviceContext.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// ������� �����ͷ� ����Ÿ�� �� ����
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr);
	if (FAILED(hr))
	{
		return false;
	}

	// ������� �����ͷ� ����Ÿ�� �� ����
	hr = m_pd3dDevice->CreateRenderTargetView(BackBufferPtr, NULL, m_RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// ����� ������ �Ҵ� ����
	BackBufferPtr->Release();
	BackBufferPtr = 0;


	// ���̹��� = Z����, z���� ����� 3d�������� ��������� �׷������� ����.
	// ���� ������ ����ü �ʱ�ȭ
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// ���̹��� ����ü ����
	// D3D11_BIND_DEPTH_STENCIL ���ٽ� ���۴� ��Ǻ�, ����޵帯 �׸��� ���� ȿ���� �� �� ���
	DepthBufferDesc.Width = ScreenWidth;
	DepthBufferDesc.Height = ScreenHeight;
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthBufferDesc.SampleDesc.Count = 1;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	// ������ ����ü�� ���̹��� �ؽ��� ����
	hr = m_pd3dDevice->CreateTexture2D(&DepthBufferDesc, NULL, m_DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// ���� ���ٽǹ��� ����ü �ʱ�ȭ
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));

	// ���ٽ� ����ü ����
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	DepthStencilDesc.StencilEnable = true;
	DepthStencilDesc.StencilReadMask = 0xFF;
	DepthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ��� ������ ���ϴ� ��� ���ٽ� �۾�
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ��� �ĸ��� ���ϴ� ��� ���ٽ� �۾�
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ������ ���Ľ� ����ü�� ����-���Ľ�State ����
	hr = m_pd3dDevice->CreateDepthStencilState(&DepthStencilDesc, m_DepthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// ���ؽ�Ʈ�� ����-���ٽ� State ����
	m_pDeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);

	// ���� ���Ľ� �並 �����ؾ� ���̹��۸� ����-���Ľ� �ؽ��ķ� �ν��Ѵ�.
	// ���� ���Ľ� ���� ����ü �ʱ�ȭ
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// ���� ���Ľ� �� ����ü ����
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;


	// ������ ���� ���Ľ� �� ����
	hr = m_pd3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &DepthStencilViewDesc, m_DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// ����Ÿ�� ��� ����-���Ľ� ���۸� ���� ��� ���������ο� ���ε�
	// ������������ �̿��� �������� �� ��, �����ߴ� ����ۿ� ����� ������ �ǰ�,
	// ����ۿ� ������ �� ���� ����Ʈ ���ۿ� �����Ͽ� ����Ϳ� ���.
	m_pDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());


	// Rasterizer ����ü �ʱ�ȭ
	ZeroMemory(&RasterDesc, sizeof(RasterDesc));

	// Rasterizer ����ü ����
	// ������ ��� �ȼ��� �׷��������� ���� ��� �� �� ����.
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_BACK;
	RasterDesc.FrontCounterClockwise = false;
	RasterDesc.DepthBias = 0;
	RasterDesc.DepthBiasClamp = 0.0f;
	RasterDesc.SlopeScaledDepthBias = 0.0f;
	RasterDesc.DepthClipEnable = true;
	RasterDesc.ScissorEnable = false;
	RasterDesc.MultisampleEnable = false;
	RasterDesc.AntialiasedLineEnable = false;

	// ������ Rasterizer ����ü ����
	hr = m_pd3dDevice->CreateRasterizerState(&RasterDesc, m_RasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// Rasterizer ���� ����
	m_pDeviceContext->RSSetState(m_RasterizerState.Get());

	// �������� ���� ����Ʈ ����
	// ������ ��ü ũ��� �����ϰ� ����
	Viewport.Width = (float)ScreenWidth;
	Viewport.Height = (float)ScreenHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;

	// ���� ��� ����
	// ��������� 3D ȭ���� 2D ����Ʈ �������� ��ȯ�ϵ��� ��.
	// �� ����� ���纻�� ���� ���̴����� ����� �׸��� ��� �ؾ� �Ѵ�.
	FieldOfView = (float)DirectX_PI / 4.0f;
	ScreenAspect = (float)ScreenWidth / (float)ScreenHeight;

	// 3D �������� ���� ������� ����
	m_matProjection = CreateProjectionMatrix(FieldOfView, ScreenAspect, screenNear, screenDepth);


	// ���� ��� ����
	// ���� ����� ������Ʈ ��ǥ�� 3D ��ǥ�� ��ȯ�� �� ���
	// ����, 3���� �������� ȸ��/�̵�/ũ�� ��ȯ���� �����.
	// ������� ���� ���纻�� �̿��� ���̴��� �����Ͽ� ����ϰ� �Ѵ�.

	// ���� ����� ���� ��ķ� �ʱ�ȭ
	m_matWorld.Identity;


	// ���� ������� ����
	// 3D ��ü�� �ƴ϶� UI ���� 2D ��Ҹ� �׸��� ���� ���
	m_matOrthoMatrix = CreateOrthoMatrix(ScreenWidth, ScreenHeight, screenNear, screenDepth);


	return true;
}

bool D3dClass::PreRender(float red, float green, float blue, float alpha)
{
	float color[4];

	// ���� ���� ����
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// ������� ������ ����
	m_pDeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);

	// ���� ���� ����
	m_pDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	return true;

}

bool D3dClass::PostRender()
{
	// �������� �Ϸ�Ǹ� ������� ������ ȭ�鿡 ǥ��
	if (m_vsync_enabled)
	{
		// ���ΰ�ħ ������ ����
		m_SwapChain->Present(1, 0);
	}
	else
	{
		// ������ �� ������ ǥ��
		m_SwapChain->Present(0, 0);
	}

	return true;
}

bool D3dClass::Release()
{
	// �����ϱ� ���� ������ ���� �ٲ��� ������ ����ü���� �Ҵ� ������ �� ���� �߻�
	if (m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

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
