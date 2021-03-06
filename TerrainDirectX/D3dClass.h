#pragma once
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d10.lib")
#pragma comment(lib, "D3DCompiler.lib")
// #pragma comment(lib, "directxtk.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "SimpleMath.h"
#include <wrl.h>
#include <fstream>
#include <string>
#include <tchar.h>
#include "dxtk/WICTextureLoader.h"
#include "dxtk/DDSTextureLoader.h"

#define DirectX_PI ((float)  3.141592654f)

static float ConvertToRadians(float fDegrees)
{
	return fDegrees * (DirectX_PI / 180.0f);
}

static float ConvertToDegrees(float fRadian)
{
	return fRadian * (180.0f / DirectX_PI);
}

using namespace Microsoft;
using namespace WRL;
using namespace DirectX;
using namespace SimpleMath;
using namespace std;

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
	bool PreRender(float red, float green, float blue, float alpha);
	bool PostRender();

	Matrix CreateProjectionMatrix(float fFov, float Aspect, float fNear, float fFar);
	Matrix CreateOrthoMatrix(float Width, float Height, float fNear, float fFar);
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	Matrix GetProjectionMatrix(Matrix ProjMatrix);
	Matrix GetWorldMatrix(Matrix matWorld);
	Matrix GetOrthoMatrix(Matrix OrthoMatrix);
	void GetVideoCardInfo(char* cardName, int& memory);

public:
	D3dClass();
	~D3dClass() {}
};
