#pragma once
// 자주 사용하지 않는 API헤더를 포함하지 않게함.
// 빌드 속도 상승 효과.
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
	bool		m_Cursor; // 마우스 커서를 표시할지 말지(true면 표시.)
	LPCWSTR		m_applicationName;
	HINSTANCE	m_hinstance;
	HWND		m_hwnd;

	Input*		m_Input;
	Graphics*	m_Graphics;
};


static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


/////////////
//  글로벌 //
/////////////
static SystemClass* ApplicationHandle = 0;