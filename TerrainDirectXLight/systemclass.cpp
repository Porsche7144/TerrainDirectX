#include "systemclass.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// 윈도우가 제거되었는지 확인.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	// 윈도우가 닫히는지 확인
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	// 다른 모든 메세지들은 System클래스의 메세지 처리기에 전달.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, msg, wParam, lParam);
	}
	}
}

SystemClass::SystemClass()
{
	m_Input = NULL;
	m_Graphics = NULL;
	m_Cursor = false;
}

// 복사 생성자
SystemClass::SystemClass(const SystemClass&)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Init()
{
	int ScreenWidth, ScreenHeight;
	bool result;

	// 함수에 높이와 너비를 전달하기 전에 변수 초기화
	ScreenWidth = 0;
	ScreenHeight = 0;

	// 윈도우즈 API를 사용하여 초기화
	InitWindows(ScreenWidth, ScreenHeight);

	m_Input = new Input;
	if (!m_Input)
	{
		return false;
	}

	// Input 객체 초기화
	m_Input->Init();

	m_Graphics = new Graphics;
	if (!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Init(ScreenWidth, ScreenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

bool SystemClass::Release()
{
	// Graphics 객체 반환
	if (m_Graphics)
	{
		// m_Graphics->Release();
		delete m_Graphics;
		m_Graphics = NULL;
	}

	// Input 객체 반환
	if (m_Input)
	{
		delete m_Input;
		m_Input = NULL;
	}

	ShutDownWindows();

	return true;
}

bool SystemClass::Run()
{
	MSG msg;
	bool Exit, result;

	// 메세지 구조체 초기화
	ZeroMemory(&msg, sizeof(MSG));

	// 종료 메세지를 받을 때까지 루프순환.
	Exit = false;
	while (!Exit)
	{
		// 윈도우 메세지 처리
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 윈도우에서 종료를 요청하는 경우 빠져나감.
		if (msg.message == WM_QUIT)
		{
			Exit = true;
		}
		else
		{
			// 아니면 프레임 호출.
			result = Frame();
			if (!result)
			{
				Exit = true;
			}
		}
	}



	return true;
}

bool SystemClass::Frame()
{
	bool result;

	// Esc키를 눌러 종료
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Graphics 객체 작업 처리.
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// 키가 눌렸는지 확인
	case WM_KEYDOWN:
	{
		m_Input->KeyDown((unsigned int)wParam);
		return 0;
	}
	// 키가 떼어졌는지 확인
	case WM_KEYUP:
	{
		m_Input->KeyUp((unsigned int)wParam);
		return 0;
	}
	// 다른 메세지는 사용하지 않으므로 기본 메세지 처리기에 전달.
	default:
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
}

bool SystemClass::InitWindows(int& ScreenWidth, int& ScreenHeight)
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	DEVMODE dmScreenSetting;
	int PosX, PosY;

	// 외부 포인터를 이 객체로 설정.
	ApplicationHandle = this;

	// 인스턴스를 가져옴
	m_hinstance = GetModuleHandle(NULL);

	// 이름 설정
	m_applicationName = L"Engine";

	// 윈도우 클래스 기본 설정
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);


	// 윈도우 클래스 등록
	if (RegisterClassEx(&wc) == 0)
	{
		return false;
	}

	// 모니터 화면 해상도 정보를 가져옴.
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		// 풀스크린 모드라면 화면 크기를 데스크 톱 크기에 맞추고 색상을 32비트로
		memset(&dmScreenSetting, 0, sizeof(dmScreenSetting));
		dmScreenSetting.dmSize = sizeof(dmScreenSetting);
		dmScreenSetting.dmPelsWidth = (unsigned long)ScreenWidth;
		dmScreenSetting.dmPelsHeight = (unsigned long)ScreenHeight;
		dmScreenSetting.dmBitsPerPel = 32;
		dmScreenSetting.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이 설정
		ChangeDisplaySettings(&dmScreenSetting, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽 위로 맞춤.
		PosX = PosY = 0;
	}
	else
	{
		// 윈도우 모드라면 800x600 크기로 설정
		ScreenWidth = 800;
		ScreenHeight = 600;

		// 창을 모니터 중앙에 오도록
		PosX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
		PosY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;

		// 설정한 값을 가지고 창을 만들고 핸들을 가져옴.
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			PosX, PosY, ScreenWidth, ScreenHeight, NULL, NULL, m_hinstance, NULL);

		// 윈도우를 화면에 표시하고 포커스를 줌(포커스를 주면 프로그램이 화면 최상위로 오게됨.).
		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);

		// 마우스 커서를 표시하지 않음.
		ShowCursor(m_Cursor);
	}

	return true;
}

bool SystemClass::ShutDownWindows()
{
	// 마우스 커서 화면에 표시
	// ShowCursor(m_Cursor);

	// 풀스크린 모드를 빠져나올 때 디스플레이 설정 변경
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// 창 제거
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 인스턴스 제거
	UnregisterClass(m_applicationName, m_hinstance);

	// 외부 포인터 참조 제거
	ApplicationHandle = NULL;

	return true;
}