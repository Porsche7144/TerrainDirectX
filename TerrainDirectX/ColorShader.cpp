#include "ColorShader.h"

bool ColorShader::Init(ID3D11Device* pDevice, HWND hwnd)
{
	InitShader(pDevice, hwnd, L"colorVS.txt", L"colorPS.txt");

	return true;
}

bool ColorShader::InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps)
{
	HRESULT hr;
	ID3DBlob* errorMessage = nullptr;
	
	// 버텍스 쉐이더 코드 컴파일
	ID3DBlob* VertexShaderBuffer = nullptr;
	// D3DCompileFromFile()


	return true;
}

ColorShader::ColorShader()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayOut = nullptr;
	m_pMatrixBuffer = nullptr;
}

ColorShader::~ColorShader()
{

}
