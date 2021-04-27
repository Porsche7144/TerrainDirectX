#pragma once
#include "Camera.h"

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
	ID3D11Buffer* m_pConstantBuffer;

public:
	bool Init(ID3D11Device* pDevice, HWND hwnd);
	bool Render(ID3D11DeviceContext* pContext, int IndexCount, Matrix World, Matrix View, Matrix Proj);
	bool Release();

	bool InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps);
	bool RenderShader(ID3D11DeviceContext* pContext, int IndexCount);
	bool ReleaseShader();

	void OutputShaderErrorMessage(ID3DBlob* ErrorMessage, HWND hwnd, const WCHAR* filename);
	bool SetShaderParameters(ID3D11DeviceContext* pContext, Matrix World, Matrix View, Matrix Proj);

public:
	ColorShader();
	~ColorShader();
};

