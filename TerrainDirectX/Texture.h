#pragma once
#include "Shader.h"

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

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string filename);
	bool Release();
	bool LoadTexture(const string filename);
	ID3D11ShaderResourceView* GetTexture();

public:
	Texture();
	~Texture();
};

