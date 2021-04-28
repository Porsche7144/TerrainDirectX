#include "Texture.h"

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_TextureSRV;
}

bool Texture::LoadTexture(const string filename)
{
	// 바이너리 모드로 파일을 연다
	FILE* filePtr;
	if (fopen_s(&filePtr, filename.c_str(), "rb") != 0)
	{
		return false;
	}

	// 파일 헤더를 읽어온다
	TargaHeader targa;
	UINT count = fread(&targa, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 파일 헤더에서 중요정보를 얻어온다
	height = targa.height;
	width = targa.width;
	int bpp = targa.bpp;

	// 파일이 32비트인지 24비트인지 체크
	// 32비트일 경우만 사용한다
	if (bpp != 32)
	{
		return false;
	}

	// 32비트 이미지 데이터의 크기 계산
	int ImageSize = width * height * 4;

	// targa 이미지 데이터용 메모리 할당
	UCHAR* targaImage = new UCHAR[ImageSize];
	if (!targaImage)
	{
		return false;
	}

	// targa 이미지 데이터 로드
	count = (UINT)fread(targaImage, 1, ImageSize, filePtr);
	if (count != ImageSize)
	{
		return false;
	}

	// 파일을 닫는다
	if (fclose(filePtr) != 0)
	{
		return false;
	}

	// targa 이미지 데이터에 인덱스 초기화
	m_TextureData = new UCHAR[ImageSize];
	if (!m_TextureData)
	{
		return false;
	}

	// targa 대상 데이터 배열에 인덱스 초기화
	int index = 0;

	// targa 이미지 데이터에 인덱스 초기화
	int k = (width * height * 4) - (width * 4);

	// targa 형식이 거꾸로 저장되어있으므로 바른 순서로 targa 이미지 데이터를
	// targa 대상 배열에 복사
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_TextureData[index + 0] = targaImage[k + 2]; // 빨강
			m_TextureData[index + 1] = targaImage[k + 1]; // 녹색
			m_TextureData[index + 2] = targaImage[k + 0]; // 파랑
			m_TextureData[index + 3] = targaImage[k + 3]; // 알파

			// index를 targa 데이터로 증가시킨다
			k += 4;
			index += 4;
		}

		// targa 이미지 데이터 인덱스를 역순으로 읽은 후 열의 시작 부분에서 이전 행으로 다시 설정
		k -= (width * 8);
	}

	// 대상 배열에 복사 된 targa 이미지 데이터 해제
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

	// 텍스쳐 구조체 설정
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

	// 빈 텍스처 생성
	hr = pDevice->CreateTexture2D(&textureDesc, NULL, &m_pTexture);
	if (FAILED(hr))
	{
		return false;
	}

	// targa 이미지 데이터의 너비 사이즈 설정
	UINT rowPitch = (width * 4) * sizeof(UCHAR);

	// targa 이미지 데이터를 텍스쳐에 복사
	pContext->UpdateSubresource(m_pTexture, 0, NULL, m_TextureData, rowPitch, 0);

	// 쉐이더 리소스 뷰 구조체 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	// 텍스쳐의 쉐이더 리소스 뷰 생성
	hr = pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, &m_TextureSRV);
	if (FAILED(hr))
	{
		return false;
	}

	// 텍스쳐에 대한 밉맵 생성
	pContext->GenerateMips(m_TextureSRV);

	// 이미지 데이터가 텍스쳐에 로드 되었으므로 이미지 데이터 해제
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
