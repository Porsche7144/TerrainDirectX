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

	// 카메라 생성
	m_pCamera = new Camera;
	if (!m_pCamera)
	{
		return false;
	}
	// 카메라 위치 세팅
	m_pCamera->SetPosition(Vector3(0.0f, 0.0f, -5.0f));

	// 모델 클래스 생성
	m_pModel = new Model;
	if (!m_pModel)
	{
		return false;
	}
	// 모델 클래스 초기화
	Result = m_pModel->Init(m_D3d->GetDevice(), m_D3d->GetDeviceContext(), _T("../Image/seafloor.dds"));
	if (!Result)
	{
		MessageBox(hwnd, L"Model 오브젝트 클래스 초기화 실패", L"Init Error", MB_OK);
		return false;
	}

	// 쉐이더 클래스 생성
	m_pShader = new Shader;
	if (!m_pShader)
	{
		return false;
	}
	// 쉐이더 클래스 초기화
	Result = m_pShader->Init(m_D3d->GetDevice(), hwnd);
	if (!Result)
	{
		MessageBox(hwnd, L"Shader 클래스 초기화 실패", L"Init Error", MB_OK);
		return false;
	}

	return true;
}

bool Graphics::Release()
{
	// 객체 반환
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
	bool result;
	Matrix worldMatrix, viewMatrix, ProjMatrix;

	// 씬 그리기 전에 버퍼 초기화
	m_D3d->PreRender(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 위치를 기준으로 뷰 행렬 생성
	m_pCamera->Render();

	// 월드, 뷰, 투영 행렬을 얻어온다.
	viewMatrix = m_pCamera->GetViewMatrix(viewMatrix);
	worldMatrix = m_D3d->GetWorldMatrix(worldMatrix);
	ProjMatrix = m_D3d->GetProjectionMatrix(ProjMatrix);

	// 모델 정점과 인덱스 버퍼를 그래픽 파이프라인에 보내 렌더링
	m_pModel->Render(m_D3d->GetDeviceContext());

	// 쉐이더를 사용해 모델 렌더링
	result = m_pShader->Render(m_D3d->GetDeviceContext(),m_pModel->GetTexture() ,m_pModel->GetIndexCount(), worldMatrix, viewMatrix, ProjMatrix);
	if (!result)
	{
		return false;
	}

	// 버퍼에 그려진 씬을 화면에 출력
	m_D3d->PostRender();

	return true;
}
