#include "Graphics.h"

bool Graphics::Init(int ScreenWidth, int ScreenHeight, HWND hwnd)
{
	bool Result;

	// Direct3D ��ü ����
	m_D3d = new D3dClass;
	if (!m_D3d)
	{
		return false;
	}

	// Direct3D ��ü �ʱ�ȭ
	Result = m_D3d->Init(ScreenWidth, ScreenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!Result)
	{
		MessageBox(hwnd, L"Direct3D �ʱ�ȭ ����", L"Error", MB_OK);
		return false;
	}

	return true;
}

bool Graphics::Release()
{
	// m_D3d ��ü ��ȯ
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

	// ������ ����
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render()
{

	// �� �׸��� ���� ���� �ʱ�ȭ
	m_D3d->PreRender(0.5f, 0.5f, 0.5f, 1.0f);

	// ���ۿ� �׷��� ���� ȭ�鿡 ���
	m_D3d->PostRender();

	return true;
}
