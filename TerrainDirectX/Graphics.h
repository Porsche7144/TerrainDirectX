#pragma once
#include <windows.h>
#include "D3dClass.h"
#include "SimpleMath.h"
#include <wrl.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	D3dClass* m_D3d;

public:
	bool Init(int ScreenWidth, int ScreenHeight, HWND hwnd);
	bool Release();
	bool Frame();
	bool Render();

public:
	Graphics()
	{
		m_D3d = NULL;
	}
	~Graphics()
	{

	}
};
