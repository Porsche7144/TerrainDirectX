#pragma once
// ���� ������� �ʴ� API����� �������� �ʰ���.
// ���� �ӵ� ��� ȿ��.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "input.h"
#include "Graphics.h"


class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Init();
	bool Release();
	bool Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	bool Frame();
	bool InitWindows(int& ScreenWidth, int& ScreenHeight);
	bool ShutDownWindows();

public:
	bool		m_Cursor; // ���콺 Ŀ���� ǥ������ ����(true�� ǥ��.)
	LPCWSTR		m_applicationName;
	HINSTANCE	m_hinstance;
	HWND		m_hwnd;

	Input*		m_Input;
	Graphics*	m_Graphics;
};


static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


/////////////
//  �۷ι� //
/////////////
static SystemClass* ApplicationHandle = 0;