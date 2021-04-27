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
	int numMode, i, Numerator, Denominator, stringLength;

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
