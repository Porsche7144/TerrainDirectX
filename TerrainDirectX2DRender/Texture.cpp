#include "Texture.h"

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_TextureSRV;
}

bool Texture::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* filename)
{

	HRESULT hr;

	if (filename == NULL)
	{
		return false;
	}

	// 텍스쳐 로드
	hr = DirectX::CreateWICTextureFromFile(pDevice, filename, m_pRSV.GetAddressOf(), &m_TextureSRV);
	if (FAILED(hr))
	{
		hr = DirectX::CreateDDSTextureFromFile(pDevice, filename, m_pRSV.GetAddressOf(), &m_TextureSRV);

		if (FAILED(hr))
		{
			return false;
		}
	}


	return true;
}

bool Texture::Release()
{

	if (m_TextureSRV)
	{
		m_TextureSRV->Release();
		m_TextureSRV = 0;
	}
	if (m_pTexture)
	{
		m_pTexture->Release();
		m_pTexture = 0;
	}
	if (m_TextureData)
	{
		delete[] m_TextureData;
		m_TextureData = 0;
	}

	return true;

}

Texture::Texture()
{
	m_TextureData = nullptr;
	m_pTexture = nullptr;
	m_TextureSRV = nullptr;
}

Texture::~Texture()
{

}
