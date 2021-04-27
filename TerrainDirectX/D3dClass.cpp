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
	int numMode, i, Numerator, Denominator, stringLength;

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
