#include "Texture.h"

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_TextureSRV;
}

bool Texture::LoadTexture(const string filename)
{
	// ���̳ʸ� ���� ������ ����
	FILE* filePtr;
	if (fopen_s(&filePtr, filename.c_str(), "rb") != 0)
	{
		return false;
	}

	// ���� ����� �о�´�
	TargaHeader targa;
	UINT count = fread(&targa, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// ���� ������� �߿������� ���´�
	height = targa.height;
	width = targa.width;
	int bpp = targa.bpp;

	// ������ 32��Ʈ���� 24��Ʈ���� üũ
	// 32��Ʈ�� ��츸 ����Ѵ�
	if (bpp != 32)
	{
		return false;
	}

	// 32��Ʈ �̹��� �������� ũ�� ���
	int ImageSize = width * height * 4;

	// targa �̹��� �����Ϳ� �޸� �Ҵ�
	UCHAR* targaImage = new UCHAR[ImageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa �̹��� ������ �ε�
	count = (UINT)fread(targaImage, 1, ImageSize, filePtr);
	if (count != ImageSize)
	{
		return false;
	}

	// ������ �ݴ´�
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa �̹��� �����Ϳ� �ε��� �ʱ�ȭ
	m_TextureData = new UCHAR[ImageSize];
	if (!m_TextureData)
	{
		return false;
	}

	// targa ��� ������ �迭�� �ε��� �ʱ�ȭ
	int index = 0;

	// targa �̹��� �����Ϳ� �ε��� �ʱ�ȭ
	int k = (width * height * 4) - (width * 4);

	// targa ������ �Ųٷ� ����Ǿ������Ƿ� �ٸ� ������ targa �̹��� �����͸�
	// targa ��� �迭�� ����
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_TextureData[index + 0] = targaImage[k + 2]; // ����
			m_TextureData[index + 1] = targaImage[k + 1]; // ���
			m_TextureData[index + 2] = targaImage[k + 0]; // �Ķ�
			m_TextureData[index + 3] = targaImage[k + 3]; // ����

			// index�� targa �����ͷ� ������Ų��
			k += 4;
			index += 4;
		}

		// targa �̹��� ������ �ε����� �������� ���� �� ���� ���� �κп��� ���� ������ �ٽ� ����
		k -= (width * 8);
	}

	// ��� �迭�� ���� �� targa �̹��� ������ ����
	delete[] targaImage;
	targaImage = 0;


	return true;
}

bool Texture::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string filename)
{

	HRESULT hr;

	if (!LoadTexture(filename.c_str()))
	{
		return false;
	}

	// �ؽ��� ����ü ����
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// �� �ؽ�ó ����
	hr = pDevice->CreateTexture2D(&textureDesc, NULL, &m_pTexture);
	if (FAILED(hr))
	{
		return false;
	}

	// targa �̹��� �������� �ʺ� ������ ����
	UINT rowPitch = (width * 4) * sizeof(UCHAR);

	// targa �̹��� �����͸� �ؽ��Ŀ� ����
	pContext->UpdateSubresource(m_pTexture, 0, NULL, m_TextureData, rowPitch, 0);

	// ���̴� ���ҽ� �� ����ü ����
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// �ؽ����� ���̴� ���ҽ� �� ����
	hr = pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_TextureSRV);
	if (FAILED(hr))
	{
		return false;
	}

	// �ؽ��Ŀ� ���� �Ӹ� ����
	pContext->GenerateMips(m_TextureSRV);

	// �̹��� �����Ͱ� �ؽ��Ŀ� �ε� �Ǿ����Ƿ� �̹��� ������ ����
	delete[] m_TextureData;
	m_TextureData = 0;


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
