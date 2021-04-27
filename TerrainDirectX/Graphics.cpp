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

	// ī�޶� ����
	m_pCamera = new Camera;
	if (!m_pCamera)
	{
		return false;
	}
	// ī�޶� ��ġ ����
	m_pCamera->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

	// �� Ŭ���� ����
	m_pModel = new Model;
	if (!m_pModel)
	{
		return false;
	}
	// �� Ŭ���� �ʱ�ȭ
	Result = m_pModel->Init(m_D3d->GetDevice());
	if (!Result)
	{
		MessageBox(hwnd, L"Model ������Ʈ Ŭ���� �ʱ�ȭ ����", L"Init Error", MB_OK);
		return false;
	}

	// ���̴� Ŭ���� ����
	m_pColorShader = new ColorShader;
	if (!m_pColorShader)
	{
		return false;
	}
	// ���̴� Ŭ���� �ʱ�ȭ
	Result = m_pColorShader->Init(m_D3d->GetDevice(), hwnd);
	if (!Result)
	{
		MessageBox(hwnd, L"Shader Ŭ���� �ʱ�ȭ ����", L"Init Error", MB_OK);
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
	if (m_pCamera)
	{
		m_pCamera->Release();
		delete m_pCamera;
		m_pCamera = NULL;
	}
	if (m_pModel)
	{
		m_pModel->Release();
		delete m_pModel;
		m_pModel = NULL;
	}
	if (m_pColorShader)
	{
		m_pColorShader->Release();
		delete m_pColorShader;
		m_pColorShader = NULL;
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
	bool result;
	Matrix worldMatrix, viewMatrix, ProjMatrix;

	// �� �׸��� ���� ���� �ʱ�ȭ
	m_D3d->PreRender(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� �������� �� ��� ����
	m_pCamera->Render();

	// ����, ��, ���� ����� ���´�.
	viewMatrix = m_pCamera->GetViewMatrix(viewMatrix);
	worldMatrix = m_D3d->GetWorldMatrix(worldMatrix);
	ProjMatrix = m_D3d->GetProjectionMatrix(ProjMatrix);

	// �� ������ �ε��� ���۸� �׷��� ���������ο� ���� ������
	m_pModel->Render(m_D3d->GetDeviceContext());

	// ���̴��� ����� �� ������
	result = m_pColorShader->Render(m_D3d->GetDeviceContext(), m_pModel->GetIndexCount(), worldMatrix, viewMatrix, ProjMatrix);
	if (!result)
	{
		return false;
	}

	// ���ۿ� �׷��� ���� ȭ�鿡 ���
	m_D3d->PostRender();

	return true;
}
