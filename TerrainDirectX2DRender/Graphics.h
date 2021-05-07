#pragma once
#include "IncludeStd.h"
#include "D3dClass.h"
#include "BitmapClass.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "TextureShader.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	D3dClass* m_D3d;
	Camera* m_pCamera;
	BitmapClass* m_pBitmap;
	Texture* m_pTexture;
	Light* m_pLight;
	TextureShader* m_pTextureShader;


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
		m_pBitmap = nullptr;
		m_pTexture = nullptr;
		m_pLight = nullptr;
	}
	~Graphics()
	{

	}
};
