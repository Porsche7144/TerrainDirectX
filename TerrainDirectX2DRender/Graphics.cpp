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
	m_pBitmap = new BitmapClass;
	if (!m_pBitmap)
	{
		return false;
	}
	// ��Ʈ�� Ŭ���� �ʱ�ȭ
	Result = m_pBitmap->Init(m_D3d->GetDevice(), m_D3d->GetDeviceContext(), _T("../Image/stone.dds"), 
							 ScreenWidth, ScreenHeight, 256, 256);
	if (!Result)
	{
		MessageBox(hwnd, L"Model ������Ʈ Ŭ���� �ʱ�ȭ ����", L"Init Error", MB_OK);
		return false;
	}

	// ���̴� Ŭ���� ����
	m_pTextureShader = new TextureShader;
	if (!m_pTextureShader)
	{
		return false;
	}
	// ���̴� Ŭ���� �ʱ�ȭ
	Result = m_pTextureShader->Init(m_D3d->GetDevice(), hwnd);
	if (!Result)
	{
		MessageBox(hwnd, L"Shader Ŭ���� �ʱ�ȭ ����", L"Init Error", MB_OK);
		return false;
	}

	// ����Ʈ Ŭ���� ����
	m_pLight = new Light;
	if (!m_pLight)
	{
		return false;
	}
	// ����Ʈ �÷�, ���� �ʱ�ȭ
	// �ֺ����� ��⸦ 15%��ŭ
	m_pLight->SetAmbientColor(Vector4(0.15f, 0.15f, 0.15f, 1.0f));
	m_pLight->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pLight->SetDirection(Vector3(0.0f, 0.0f, 1.0f));
	// �ݻ��
	m_pLight->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	// �ݻ簭��. �ݻ簭�� ���� �������� �ݻ籤 ȿ���� Ŀ����.
	m_pLight->SetSpecularPower(32.0f);

	return true;
}

bool Graphics::Release()
{
	// ��ü ��ȯ
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
	if (m_pBitmap)
	{
		m_pBitmap->Release();
		delete m_pBitmap;
		m_pBitmap = NULL;
	}
	if (m_pTextureShader)
	{
		m_pTextureShader->Release();
		delete m_pTextureShader;
		m_pTextureShader = NULL;
	}
	if (m_pLight)
	{
		delete m_pLight;
		m_pLight = NULL;
	}

	return true;
}

bool Graphics::Frame()
{
	bool result;

	// �� �����Ӹ��� �޶����� ȸ�������� �����ϱ� ���� static ���� �߰�
	static float rotation = 0.0f;

	rotation += (float)DirectX_PI * 0.00000f;
	if (rotation >= 360.0f)
	{
		rotation -= 360.0f;
	}


	// ������ ����
	result = Render(rotation);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Graphics::Render(float rotation)
{
	bool result;
	Matrix worldMatrix, viewMatrix, ProjMatrix, orthoMatrix;

	// �� �׸��� ���� ���� �ʱ�ȭ
	m_D3d->PreRender(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� �������� �� ��� ����
	m_pCamera->Render();

	// ����, ��, ���� ����� ���´�.
	viewMatrix = m_pCamera->GetViewMatrix(viewMatrix);
	worldMatrix = m_D3d->GetWorldMatrix(worldMatrix);
	ProjMatrix = m_D3d->GetProjectionMatrix(ProjMatrix);
	orthoMatrix = m_D3d->GetOrthoMatrix(orthoMatrix);

	// 2D ������ ���� Z���� Off
	m_D3d->OffDepthBuffer();

	// ��������� ȸ���� ��ŭ ȸ������ �ﰢ���� ������ �� �� �� ����ŭ ȸ���Ǿ� ���̰� �Ѵ�.
	worldMatrix = worldMatrix.CreateRotationY(rotation);

	// �� ������ �ε��� ���۸� �׷��� ���������ο� ���� ������
	// ȭ�� 100, 100 ��ġ�� ��Ʈ�� ������
	m_pBitmap->Render(m_D3d->GetDeviceContext(), 100, 100);

	// ���̴��� ����� �� ������
	result = m_pTextureShader->Render(m_D3d->GetDeviceContext(), m_pBitmap->GetTexture() , m_pBitmap->GetIndexCount(),
								worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// ���ۿ� �׷��� ���� ȭ�鿡 ���
	m_D3d->PostRender();

	return true;
}
