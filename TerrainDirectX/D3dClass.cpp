#include "D3dClass.h"

// 디바이스와 컨텍스트의 포인터를 가져오는 함수
ID3D11Device* D3dClass::GetDevice()
{
	return m_pd3dDevice.Get();
}

ID3D11DeviceContext* D3dClass::GetDeviceContext()
{
	return m_pDeviceContext.Get();
}

// 투영, 월드, 직교 투영행렬을 반환하는 함수
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

// 그래픽카드 이름과 사용가능한 메모리 양 반환(디버깅용)
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


	// vsync(수직동기화) 설정 저장.
	// 수직동기화 = 렌더링을 모니터 주사율에 맞출 것인지, 가능한 한 빠르게 다시 그릴것인지
	m_vsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리 생성
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&Factory);
	if (FAILED(hr))
	{
		return false;
	}

	// 팩토리 객체를 사용해 첫번째 그래픽 카드 인터페이스에 대한 어댑터 생성
	hr = Factory->EnumAdapters(0, &Adapter);
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

	// DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 모드 개수를 구함
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numMode, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	// 가능한 모든 모니터와 그래픽 카드 조합을 저장할 리스트 생성
	DisplayModeList = new DXGI_MODE_DESC[numMode];
	if (!DisplayModeList)
	{
		return false;
	}

	// 디스플레이 모드에 대한 리스트 구조를 채움
	hr = AdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numMode, DisplayModeList);
	if (FAILED(hr))
	{
		return false;
	}

	// 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드 검색.
	// 적합한 것을 찾으면 모니터의 새로고침 비율 저장.
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

	// 어댑터(그래픽카드)의 구조체를 가져옴
	hr = Adapter->GetDesc(&AdapterDesc);
	if (FAILED(hr))
	{
		return false;
	}

	// 현재 그래픽카드의 메모리 용량을 메가바이트 단위로 저장
	m_VideoCardMemory = (int)(AdapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 그래픽카드의 이름을 char 문자열 배열로 저장
	error = wcstombs_s(&stringLength, m_VideoCardDescription, 128, AdapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// 디스플레이 모드 리스트의 할당 해제
	delete[] DisplayModeList;
	DisplayModeList = 0;

	// 출력 어댑터 할당 해제
	AdapterOutput->Release();
	AdapterOutput = 0;

	// 어댑터 할당 해제
	Adapter->Release();
	Adapter = 0;

	// 핵토리 할당 해제
	Factory->Release();
	Factory = 0;


	// 스왑체인 = 렌더링 할 때 백버퍼와 프론트버를 교체하기 위해 사용.
	// 스왑체인 구조체 초기화
	ZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	// 하나의 백버퍼 사용하도록
	SwapChainDesc.BufferCount = 1;

	// 백버퍼 너비/높이 설정
	SwapChainDesc.BufferDesc.Width = ScreenWidth;
	SwapChainDesc.BufferDesc.Height = ScreenHeight;

	// 백버퍼로 서페이스 지정
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼 새로고침 비율 설정
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

	// 백버퍼의 용도 설정
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링이 이루어질 윈도우 핸들 설정
	SwapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링 Off
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;

	// 윈도우 모드 또는 풀스크린 모드 설정
	if (fullscreen)
	{
		SwapChainDesc.Windowed = false;
	}
	else
	{
		SwapChainDesc.Windowed = true;
	}

	// 스캔라인의 정렬과 스캔라이닝을 지정되지 않음(UNSPECIFIED)으로 설정
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 이후 백버퍼의 내용을 버리도록
	SwapChainDesc.SwapEffect - DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그 사용 안함.
	SwapChainDesc.Flags = 0;
	
	// 피쳐레벨을 DirectX 11로 설정
	FeatureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑체인, Direct3D 디바이스, 컨텍스트 생성
	// D3D_DRIVER_TYPE_HARDWARE를 D3D_DRIVER_TYPE_REFERENCE 로 바꾸어 설정하면
	// 그래픽카드가 아닌 CPU에서 렌더링 처리. (약 1/1000 정도로 느림.)
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &FeatureLevel, 1, D3D11_SDK_VERSION, 
		&SwapChainDesc,	m_SwapChain.GetAddressOf(), m_pd3dDevice.GetAddressOf(), NULL, m_pDeviceContext.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 백버퍼의 포인터로 렌더타겟 뷰 생성
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr);
	if (FAILED(hr))
	{
		return false;
	}

	// 백버퍼의 포인터로 렌더타겟 뷰 생성
	hr = m_pd3dDevice->CreateRenderTargetView(BackBufferPtr, NULL, m_RenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 백버퍼 포인터 할당 해제
	BackBufferPtr->Release();
	BackBufferPtr = 0;


	// 깊이버퍼 = Z버퍼, z값을 사용해 3d공간에서 폴리곤들이 그려지도록 설정.
	// 깊이 버퍼의 구조체 초기화
	ZeroMemory(&DepthBufferDesc, sizeof(DepthBufferDesc));

	// 깊이버퍼 구조체 설정
	// D3D11_BIND_DEPTH_STENCIL 스텐실 버퍼는 모션블러, 블류메드릭 그림자 등의 효과를 낼 때 사용
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

	// 설정한 구조체로 깊이버퍼 텍스쳐 생성
	hr = m_pd3dDevice->CreateTexture2D(&DepthBufferDesc, NULL, m_DepthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 깊이 스텐실버퍼 구조체 초기화
	ZeroMemory(&DepthStencilDesc, sizeof(DepthStencilDesc));

	// 스텐실 구조체 설정
	DepthStencilDesc.DepthEnable = true;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	DepthStencilDesc.StencilEnable = true;
	DepthStencilDesc.StencilReadMask = 0xFF;
	DepthStencilDesc.StencilWriteMask = 0xFF;

	// 픽셀이 전면을 향하는 경우 스텐실 작업
	DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 픽셀이 후면을 향하는 경우 스텐실 작업
	DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 설정한 스탠실 구조체로 깊이-스탠실State 생성
	hr = m_pd3dDevice->CreateDepthStencilState(&DepthStencilDesc, m_DepthStencilState.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 컨텍스트로 깊이-스텐실 State 적용
	m_pDeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);

	// 깊이 스탠실 뷰를 설정해야 깊이버퍼를 깊이-스탠실 텍스쳐로 인식한다.
	// 깊이 스탠실 뷰의 구조체 초기화
	ZeroMemory(&DepthStencilViewDesc, sizeof(DepthStencilViewDesc));

	// 깊이 스탠실 뷰 구조체 설정
	DepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0;


	// 설정한 깊이 스탠실 뷰 생성
	hr = m_pd3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), &DepthStencilViewDesc, m_DepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// 렌더타겟 뷰와 깊이-스탠실 버퍼를 각각 출력 파이프라인에 바인딩
	// 파이프라인을 이용한 렌더링을 할 때, 생성했던 백버퍼에 장면이 렌더링 되고,
	// 백버퍼에 렌더링 된 것을 프론트 버퍼와 스왑하여 모니터에 출력.
	m_pDeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());


	// Rasterizer 구조체 초기화
	ZeroMemory(&RasterDesc, sizeof(RasterDesc));

	// Rasterizer 구조체 설정
	// 도형이 어떻게 픽셀로 그려지는지에 대한 제어를 할 수 있음.
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

	// 설정한 Rasterizer 구조체 생성
	hr = m_pd3dDevice->CreateRasterizerState(&RasterDesc, m_RasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	// Rasterizer 상태 적용
	m_pDeviceContext->RSSetState(m_RasterizerState.Get());

	// 렌더링을 위한 뷰포트 설정
	// 윈도우 전체 크기와 동일하게 설정
	Viewport.Width = (float)ScreenWidth;
	Viewport.Height = (float)ScreenHeight;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;

	// 투영 행렬 생성
	// 투영행렬은 3D 화면을 2D 뷰포트 공간으로 변환하도록 함.
	// 이 행렬의 복사본을 만들어서 셰이더에서 장면을 그릴때 사용 해야 한다.
	FieldOfView = (float)DirectX_PI / 4.0f;
	ScreenAspect = (float)ScreenWidth / (float)ScreenHeight;

	// 3D 렌더링을 위한 투영행렬 생성
	m_matProjection = CreateProjectionMatrix(FieldOfView, ScreenAspect, screenNear, screenDepth);


	// 월드 행렬 생성
	// 월드 행렬은 오브젝트 좌표를 3D 좌표로 변환할 때 사용
	// 또한, 3차원 공간에서 회전/이동/크기 변환에도 사용함.
	// 월드행렬 역시 복사본을 이용해 셰이더에 전달하여 사용하게 한다.

	// 월드 행렬을 단위 행렬로 초기화
	m_matWorld.Identity;


	// 직교 투영행렬 생성
	// 3D 객체가 아니라 UI 같은 2D 요소를 그리기 위해 사용
	m_matOrthoMatrix = CreateOrthoMatrix(ScreenWidth, ScreenHeight, screenNear, screenDepth);


	return true;
}

bool D3dClass::PreRender(float red, float green, float blue, float alpha)
{
	float color[4];

	// 버퍼 색상 설정
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 백버퍼의 내용을 삭제
	m_pDeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);

	// 깊이 버퍼 삭제
	m_pDeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	return true;

}

bool D3dClass::PostRender()
{
	// 렌더링이 완료되면 백버퍼의 내용을 화면에 표시
	if (m_vsync_enabled)
	{
		// 새로고침 비율을 고정
		m_SwapChain->Present(1, 0);
	}
	else
	{
		// 가능한 한 빠르게 표시
		m_SwapChain->Present(0, 0);
	}

	return true;
}

bool D3dClass::Release()
{
	// 종료하기 전에 윈도우 모드로 바꾸지 않으면 스왑체인을 할당 해제할 때 예외 발생
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
