#pragma once
#include "Camera.h"

struct MatrixBufferType
{
	Matrix world;
	Matrix view;
	Matrix Projection;
};

struct LightBufferType
{
	Vector4 AmbientColor;
	Vector4 DiffuseColor;
	Vector3 LightDirection;

	// 구조체의 크기가 16의 배수가 되게 하기 위해서 사용하지 않는 float 변수 하나 추가.
	// 만약 추가한 변수 없이 sizeof(LightBufferType)가 28바이트인 크기로
	// CreateBuffer를 하면, CreateBuffer에서 무조건 16의 배수 크기를 요구하기 때문에 실패할 것.
	float Padding;
};

class Shader
{
public:
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pLayOut;
	ID3D11Buffer* m_pConstantBuffer;
	ID3D11Buffer* m_pLightConstantBuffer;
	ID3D11SamplerState* m_pSamplerState;

public:
	bool Init(ID3D11Device* pDevice, HWND hwnd);
	bool Render(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* texture, int IndexCount, 
					Matrix World, Matrix View, Matrix Proj, Vector3 LightDir, Vector4 DiffuseColor, Vector4 AmbientColor);
	bool Release();

	bool InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps);
	bool RenderShader(ID3D11DeviceContext* pContext, int IndexCount);
	bool ReleaseShader();

	void OutputShaderErrorMessage(ID3DBlob* ErrorMessage, HWND hwnd, const WCHAR* filename);
	bool SetShaderParameters(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* texture,
					Matrix World, Matrix View, Matrix Proj, Vector3 LightDir, Vector4 DiffuseColor, Vector4 AmbientColor);

public:
	Shader();
	~Shader();
};

