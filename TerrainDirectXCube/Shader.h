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

	// ����ü�� ũ�Ⱑ 16�� ����� �ǰ� �ϱ� ���ؼ� ������� �ʴ� float ���� �ϳ� �߰�.
	// ���� �߰��� ���� ���� sizeof(LightBufferType)�� 28����Ʈ�� ũ���
	// CreateBuffer�� �ϸ�, CreateBuffer���� ������ 16�� ��� ũ�⸦ �䱸�ϱ� ������ ������ ��.
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

