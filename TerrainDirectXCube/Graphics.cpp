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
	Result = m_pModel->Init(m_D3d->GetDevice(), m_D3d->GetDeviceContext(), _T("../Image/stone.dds"), _T("../data/cube.txt"));
	if (!Result)
	{
		MessageBox(hwnd, L"Model ������Ʈ Ŭ���� �ʱ�ȭ ����", L"Init Error", MB_OK);
		return false;
	}

	// ���̴� Ŭ���� ����
	m_pShader = new Shader;
	if (!m_pShader)
	{
		return false;
	}
	// ���̴� Ŭ���� �ʱ�ȭ
	Result = m_pShader->Init(m_D3d->GetDevice(), hwnd);
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
	if (m_pModel)
	{
		m_pModel->Release();
		delete m_pModel;
		m_pModel = NULL;
	}
	if (m_pShader)
	{
		m_pShader->Release();
		delete m_pShader;
		m_pShader = NULL;
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

	rotation += (float)DirectX_PI * 0.00005f;
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
	Matrix worldMatrix, viewMatrix, ProjMatrix;

	// �� �׸��� ���� ���� �ʱ�ȭ
	m_D3d->PreRender(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶� ��ġ�� �������� �� ��� ����
	m_pCamera->Render();

	// ����, ��, ���� ����� ���´�.
	viewMatrix = m_pCamera->GetViewMatrix(viewMatrix);
	worldMatrix = m_D3d->GetWorldMatrix(worldMatrix);
	ProjMatrix = m_D3d->GetProjectionMatrix(ProjMatrix);

	// ��������� ȸ���� ��ŭ ȸ������ �ﰢ���� ������ �� �� �� ����ŭ ȸ���Ǿ� ���̰� �Ѵ�.
	worldMatrix = worldMatrix.CreateRotationY(rotation);

	// �� ������ �ε��� ���۸� �׷��� ���������ο� ���� ������
	m_pModel->Render(m_D3d->GetDeviceContext());

	// ���̴��� ����� �� ������
	result = m_pShader->Render(m_D3d->GetDeviceContext(),m_pModel->GetTexture() ,m_pModel->GetIndexCount(), 
								worldMatrix, viewMatrix, ProjMatrix, 
								m_pLight->GetDirection(), m_pLight->GetDiffuseColor(), m_pLight->GetAmbientColor(),
								m_pCamera->GetPosition(), m_pLight->GetSpecularPower(), m_pLight->GetSpecularColor());
	if (!result)
	{
		return false;
	}

	// ���ۿ� �׷��� ���� ȭ�鿡 ���
	m_D3d->PostRender();

	return true;
}
