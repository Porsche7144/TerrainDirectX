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
	m_pCamera->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

	// 모델 클래스 생성
	m_pBitmap = new BitmapClass;
	if (!m_pBitmap)
	{
		return false;
	}
	// 비트맵 클래스 초기화
	Result = m_pBitmap->Init(m_D3d->GetDevice(), m_D3d->GetDeviceContext(), _T("../Image/stone.dds"), 
							 ScreenWidth, ScreenHeight, 256, 256);
	if (!Result)
	{
		MessageBox(hwnd, L"Model 오브젝트 클래스 초기화 실패", L"Init Error", MB_OK);
		return false;
	}

	// 쉐이더 클래스 생성
	m_pTextureShader = new TextureShader;
	if (!m_pTextureShader)
	{
		return false;
	}
	// 쉐이더 클래스 초기화
	Result = m_pTextureShader->Init(m_D3d->GetDevice(), hwnd);
	if (!Result)
	{
		MessageBox(hwnd, L"Shader 클래스 초기화 실패", L"Init Error", MB_OK);
		return false;
	}

	// 라이트 클래스 생성
	m_pLight = new Light;
	if (!m_pLight)
	{
		return false;
	}
	// 라이트 컬러, 방향 초기화
	// 주변광의 밝기를 15%만큼
	m_pLight->SetAmbientColor(Vector4(0.15f, 0.15f, 0.15f, 1.0f));
	m_pLight->SetDiffuseColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_pLight->SetDirection(Vector3(0.0f, 0.0f, 1.0f));
	// 반사색
	m_pLight->SetSpecularColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	// 반사강도. 반사강도 값이 낮을수록 반사광 효과가 커진다.
	m_pLight->SetSpecularPower(32.0f);

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

	// 매 프라임마다 달라지는 회전정보를 저장하기 위해 static 변수 추가
	static float rotation = 0.0f;

	rotation += (float)DirectX_PI * 0.00000f;
	if (rotation >= 360.0f)
	{
		rotation -= 360.0f;
	}


	// 렌더링 수행
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

	// 씬 그리기 전에 버퍼 초기화
	m_D3d->PreRender(0.0f, 0.0f, 0.0f, 1.0f);

	// 카메라 위치를 기준으로 뷰 행렬 생성
	m_pCamera->Render();

	// 월드, 뷰, 투영 행렬을 얻어온다.
	viewMatrix = m_pCamera->GetViewMatrix(viewMatrix);
	worldMatrix = m_D3d->GetWorldMatrix(worldMatrix);
	ProjMatrix = m_D3d->GetProjectionMatrix(ProjMatrix);
	orthoMatrix = m_D3d->GetOrthoMatrix(orthoMatrix);

	// 2D 렌더링 전에 Z버퍼 Off
	m_D3d->OffDepthBuffer();

	// 월드행렬을 회전값 만큼 회전시켜 삼각형을 렌더링 할 때 그 값만큼 회전되어 보이게 한다.
	worldMatrix = worldMatrix.CreateRotationY(rotation);

	// 모델 정점과 인덱스 버퍼를 그래픽 파이프라인에 보내 렌더링
	// 화면 100, 100 위치에 비트맵 렌더링
	m_pBitmap->Render(m_D3d->GetDeviceContext(), 100, 100);

	// 쉐이더를 사용해 모델 렌더링
	result = m_pTextureShader->Render(m_D3d->GetDeviceContext(), m_pBitmap->GetTexture() , m_pBitmap->GetIndexCount(),
								worldMatrix, viewMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// 버퍼에 그려진 씬을 화면에 출력
	m_D3d->PostRender();

	return true;
}
