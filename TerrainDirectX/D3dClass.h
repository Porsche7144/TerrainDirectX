#pragma once
#ifndef _D3dClass_H_
#define D3dClass_H_
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "D3DCompiler.lib")
// #pragma comment(lib, "directxtk.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include "SimpleMath.h"
#include <wrl.h>

using namespace Microsoft;
using namespace WRL;
using namespace DirectX;
using namespace SimpleMath;

class D3dClass
{
public:
	bool m_vsync_enabled;
	int m_VideoCardMemory;
	char m_VideoCardDescription[128];
	ComPtr<IDXGISwapChain> m_SwapChain;
	ComPtr<ID3D11Device> m_pd3dDevice;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
	ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
	ComPtr<ID3D11RasterizerState> m_RasterizerState;
	Matrix m_matProjection;
	Matrix m_matWorld;
	Matrix m_matOrthoMatrix;

public:
	bool Init(int ScreenWidth, int ScreenHeight, bool vsync, HWND hwnd, 
			   bool fullscreen, float screenDepth, float screenNear);
	bool Release();


public:
	D3dClass();
	~D3dClass() {}
};

#endif