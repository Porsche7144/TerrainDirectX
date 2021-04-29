#pragma once
#include <windows.h>
#include "D3dClass.h"
#include "SimpleMath.h"
#include <wrl.h>
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Model.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Light.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	D3dClass* m_D3d;
	Camera* m_pCamera;
	Model* m_pModel;
	Shader* m_pShader;
	Texture* m_pTexture;
	Light* m_pLight;


public:
	bool Init(int ScreenWidth, int ScreenHeight, HWND hwnd);
	bool Release();
	bool Frame();
	bool Render(float rotation);

public:
	Graphics()
	{
		m_D3d = nullptr;
		m_pCamera = nullptr;
		m_pModel = nullptr;
		m_pShader = nullptr;
		m_pTexture = nullptr;
		m_pLight = nullptr;
	}
	~Graphics()
	{

	}
};
