#pragma once
#include "Graphics.h"

struct MatrixBufferType
{
	Matrix world;
	Matrix view;
	Matrix Projection;
};

class ColorShader
{
public:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pLayOut;
	ID3D11Buffer* m_pMatrixBuffer;

public:
	bool Init(ID3D11Device* pDevice, HWND hwnd);
	bool Release();
	bool InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps);

public:
	ColorShader();
	~ColorShader();
};

