#include "Graphics.h"

bool Graphics::Init(int ScreenWidth, int ScreenHeight, HWND hwnd)
{
	bool Result;

	// Direct3D 객체 생성
	m_D3d = new D3dClass;
	if (!m_D3d)
	{
		return false;
	}

	// Direct3D 객체 초기화
	Result = m_D3d->Init(ScreenWidth, ScreenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!Result)
	{
		MessageBox(hwnd, L"Direct3D 초기화 실패", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Graphics::Release()
{
	// m_D3d 객체 반환
	if (m_D3d)
	{
		m_D3d->Release();
		delete m_D3d;
		m_D3d = NULL;
	}

	return true;
}

bool Graphics::Frame()
{
	bool result;

	// 렌더링 수행
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render()
{

	// 씬 그리기 전에 버퍼 초기화
	m_D3d->PreRender(0.5f, 0.5f, 0.5f, 1.0f);

	// 버퍼에 그려진 씬을 화면에 출력
	m_D3d->PostRender();

	return true;
}
