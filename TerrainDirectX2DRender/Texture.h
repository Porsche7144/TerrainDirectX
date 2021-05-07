#pragma once
#include "TextureShader.h"

struct TargaHeader
{
	UCHAR data1[12];
	USHORT width;
	USHORT height;
	UCHAR bpp;
	UCHAR data2;
};

class Texture
{
public:
	int TextureWidth = 0;
	int TextureHeight = 0;

	string* m_TextureData;
	ID3D11Texture2D* m_pTexture;
	ID3D11ShaderResourceView* m_TextureSRV;
	ComPtr<ID3D11Resource> m_pRSV;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* filename);
	bool Release();
	ID3D11ShaderResourceView* GetTexture();

public:
	Texture();
	~Texture();
};

