#include "systemclass.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// �����찡 ���ŵǾ����� Ȯ��.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	// �����찡 �������� Ȯ��
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	// �ٸ� ��� �޼������� SystemŬ������ �޼��� ó���⿡ ����.
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

// ���� ������
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

	// �Լ��� ���̿� �ʺ� �����ϱ� ���� ���� �ʱ�ȭ
	ScreenWidth = 0;
	ScreenHeight = 0;

	// �������� API�� ����Ͽ� �ʱ�ȭ
	InitWindows(ScreenWidth, ScreenHeight);

	m_Input = new Input;
	if (!m_Input)
	{
		return false;
	}

	// Input ��ü �ʱ�ȭ
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
	// Graphics ��ü ��ȯ
	if (m_Graphics)
	{
		// m_Graphics->Release();
		delete m_Graphics;
		m_Graphics = NULL;
	}

	// Input ��ü ��ȯ
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

	// �޼��� ����ü �ʱ�ȭ
	ZeroMemory(&msg, sizeof(MSG));

	// ���� �޼����� ���� ������ ������ȯ.
	Exit = false;
	while (!Exit)
	{
		// ������ �޼��� ó��
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// �����쿡�� ���Ḧ ��û�ϴ� ��� ��������.
		if (msg.message == WM_QUIT)
		{
			Exit = true;
		}
		else
		{
			// �ƴϸ� ������ ȣ��.
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

	// EscŰ�� ���� ����
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// Graphics ��ü �۾� ó��.
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
		// Ű�� ���ȴ��� Ȯ��
	case WM_KEYDOWN:
	{
		m_Input->KeyDown((unsigned int)wParam);
		return 0;
	}
	// Ű�� ���������� Ȯ��
	case WM_KEYUP:
	{
		m_Input->KeyUp((unsigned int)wParam);
		return 0;
	}
	// �ٸ� �޼����� ������� �����Ƿ� �⺻ �޼��� ó���⿡ ����.
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

	// �ܺ� �����͸� �� ��ü�� ����.
	ApplicationHandle = this;

	// �ν��Ͻ��� ������
	m_hinstance = GetModuleHandle(NULL);

	// �̸� ����
	m_applicationName = L"Engine";

	// ������ Ŭ���� �⺻ ����
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


	// ������ Ŭ���� ���
	if (RegisterClassEx(&wc) == 0)
	{
		return false;
	}

	// ����� ȭ�� �ػ� ������ ������.
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		// Ǯ��ũ�� ����� ȭ�� ũ�⸦ ����ũ �� ũ�⿡ ���߰� ������ 32��Ʈ��
		memset(&dmScreenSetting, 0, sizeof(dmScreenSetting));
		dmScreenSetting.dmSize = sizeof(dmScreenSetting);
		dmScreenSetting.dmPelsWidth = (unsigned long)ScreenWidth;
		dmScreenSetting.dmPelsHeight = (unsigned long)ScreenHeight;
		dmScreenSetting.dmBitsPerPel = 32;
		dmScreenSetting.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Ǯ��ũ���� �´� ���÷��� ����
		ChangeDisplaySettings(&dmScreenSetting, CDS_FULLSCREEN);

		// �������� ��ġ�� ȭ���� ���� ���� ����.
		PosX = PosY = 0;
	}
	else
	{
		// ������ ����� 800x600 ũ��� ����
		ScreenWidth = 800;
		ScreenHeight = 600;

		// â�� ����� �߾ӿ� ������
		PosX = (GetSystemMetrics(SM_CXSCREEN) - ScreenWidth) / 2;
		PosY = (GetSystemMetrics(SM_CYSCREEN) - ScreenHeight) / 2;

		// ������ ���� ������ â�� ����� �ڵ��� ������.
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			PosX, PosY, ScreenWidth, ScreenHeight, NULL, NULL, m_hinstance, NULL);

		// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� ��(��Ŀ���� �ָ� ���α׷��� ȭ�� �ֻ����� ���Ե�.).
		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);

		// ���콺 Ŀ���� ǥ������ ����.
		ShowCursor(m_Cursor);
	}

	return true;
}

bool SystemClass::ShutDownWindows()
{
	// ���콺 Ŀ�� ȭ�鿡 ǥ��
	// ShowCursor(m_Cursor);

	// Ǯ��ũ�� ��带 �������� �� ���÷��� ���� ����
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// â ����
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// �ν��Ͻ� ����
	UnregisterClass(m_applicationName, m_hinstance);

	// �ܺ� ������ ���� ����
	ApplicationHandle = NULL;

	return true;
}