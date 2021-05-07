#pragma once
#include "Texture.h"

struct FontType
{
	float left, right;
	int size;
};

struct FontVertexType
{
	Vector3 pos;
	Vector2 textureUV;
};

class FontClass
{
public:
	FontType* m_pFontType;
	Texture* m_pTexture;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* fontFilename, const WCHAR* textrueFileName);
	bool Release();
	bool LoadFontData(const char* fontFileName);
	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* textureFileName);
	ID3D11ShaderResourceView* GetFontTexture();
	bool BuildVertexArray(void* vertices, const char* sentence, float DrawX, float DrawY);

public:
	FontClass();
	~FontClass();
};

