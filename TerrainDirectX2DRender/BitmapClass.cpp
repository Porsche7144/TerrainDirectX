#include "BitmapClass.h"

BitmapClass::BitmapClass()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTextureClass = nullptr;
	// m_pModelType = nullptr;
}

// 모델의 인덱스 개수를 알려주는 함수
int BitmapClass::GetIndexCount()
{
	return m_iIndexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_pTextureClass->GetTexture();
}

bool BitmapClass::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename)
{
	// 텍스쳐 오브젝트 생성
	m_pTextureClass = new Texture;
	if (!m_pTextureClass)
	{
		return false;
	}

	// 텍스쳐 오브젝트 초기화
	if (!m_pTextureClass->Init(pDevice, pContext, Filename))
	{
		return false;
	}

	return true;
}

bool BitmapClass::LoadModelText(const TCHAR* texName)
{
	ifstream fin;
	char input;
	int i;

	// 텍스트 파일 오픈
	fin.open(texName);

	// 파일 열기에 실패하면 나가기
	if (fin.fail())
	{
		return false;
	}

	// 정점 카운트값 까지 읽기
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 정점 카운트 개수를 읽는다
	fin >> m_iVertexCount;

	// 인덱스 수를 정점 수와 같은 수로 세팅
	m_iIndexCount = m_iVertexCount;

	// 읽어온 정점 갯수만큼 객체 생성
	//m_pModelType = new ModelType[m_iVertexCount];
	//if (!m_pModelType)
	//{
	//	return false;
	//}

	// 데이터 시작부분 까지 읽기
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 정점 정보를 읽어온다.
	//for (int i = 0; i < m_iVertexCount; i++)
	//{
	//	fin >> m_pModelType[i].x >> m_pModelType[i].y >> m_pModelType[i].z;
	//	fin >> m_pModelType[i].tu >> m_pModelType[i].tv;
	//	fin >> m_pModelType[i].nx >> m_pModelType[i].ny >> m_pModelType[i].nz;
	//}

	// 텍스트 닫기
	fin.close();

	return true;
}

bool BitmapClass::UpdateBuffer(ID3D11DeviceContext* pContext, int PosX, int PosY)
{
	float left, right, top, bottom;
	BitmapVertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	BitmapVertexType* verticesPtr;
	HRESULT hr;

	// 비트맵 렌더링 할 위치가 변경되지 않은 경우 정점버퍼를 업데이트 하지 않고 반환
	if (PosX == m_PrevPosX && PosY == m_PrevPosY)
	{
		return true;
	}
	// 그렇지 않다면 렌더링 위치 업데이트
	else
	{
		m_PrevPosX = PosX;
		m_PrevPosY = PosY;

		// 비트맵 화면좌표 계산
		left = (float)((m_ScreenWidth / 2) * -1) + (float)PosX;
		right = left + (float)m_BitmapWidth;
		top = (float)(m_ScreenHeight / 2)  - (float)PosY;
		bottom = top - (float)m_BitmapHeight;
	}

	vertices = new BitmapVertexType[m_iVertexCount];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열 데이터 로드  첫번째 삼각형
	vertices[0].pos = Vector3(left, top, 0.0f);
	vertices[0].TextureUV = Vector2(0.0f, 0.0f);

	vertices[1].pos = Vector3(right, bottom, 0.0f);
	vertices[1].TextureUV = Vector2(1.0f, 1.0f);

	vertices[2].pos = Vector3(left, bottom, 0.0f);
	vertices[2].TextureUV = Vector2(0.0f, 1.0f);

	// 두번째 삼각형
	vertices[3].pos = Vector3(left, top, 0.0f);
	vertices[3].TextureUV = Vector2(0.0f, 0.0f);

	vertices[4].pos = Vector3(right, top, 0.0f);
	vertices[4].TextureUV = Vector2(1.0f, 0.0f);

	vertices[5].pos = Vector3(right, bottom, 0.0f);
	vertices[5].TextureUV = Vector2(1.0f, 1.0f);

	// 버텍스 버퍼를 쓰기 가능하도록 잠금
	hr = pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hr))
	{
		return false;
	}
	else
	{
		// 버텍스 버퍼의 데이터에 대한 포인터를 가져옴
		verticesPtr = (BitmapVertexType*)mappedResource.pData;
		// 데이터를 정점 버퍼에 복사
		memcpy(verticesPtr, (void*)vertices, (sizeof(BitmapVertexType) * m_iVertexCount));
	}

	pContext->Unmap(m_pVertexBuffer, 0);

	// 상수버퍼에 넘긴 후 필요하지 않은 데이터 해제
	delete[] vertices;
	vertices = 0;

	return true;
}

bool BitmapClass::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename,
						int ScreenWidht, int ScreenHeight, int bitmapWidth, int bitmapHeight)
{
	bool result;
	
	// 스크린과 비트맵 사이즈 세팅
	m_ScreenHeight = ScreenHeight;
	m_ScreenWidth = ScreenWidht;

	// 렌더링할 비트맵 픽셀단위
	m_BitmapHeight = bitmapHeight;
	m_BitmapWidth = bitmapWidth;

	// 이전 렌더링 위치를 마이너스로 초기화. 이전 위치를 기억하는 용도.
	m_PrevPosX = -1;
	m_PrevPosY = -1;


	// 정점버퍼와 인덱스버퍼 초기화
	result = InitBuffers(pDevice);
	if (!result)
	{
		return false;
	}

	result = LoadTexture(pDevice, pContext, Filename);
	if (!result)
	{
		return false;
	}

	return true;
}

bool BitmapClass::InitBuffers(ID3D11Device* pDevice)
{
	HRESULT hr;
	BitmapVertexType* vertices;
	ULONG* indices;
	D3D11_BUFFER_DESC VertexBufferDesc, IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA VertexData, IndexData;

	m_iVertexCount = 6;
	m_iIndexCount = m_iVertexCount;

	// 버텍스 배열 생성
	vertices = new BitmapVertexType[m_iVertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열 생성
	indices = new ULONG[m_iIndexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열 0으로 초기화
	memset(vertices, 0, sizeof(BitmapVertexType) * m_iVertexCount);

	// 인덱스 배열에 값 할당
	for (int i = 0; i < m_iIndexCount; i++)
	{
		indices[i] = i;
	}

	// 정점버퍼 구조체 초기화
	ZeroMemory(&VertexBufferDesc, sizeof(VertexBufferDesc));

	// 동적 정점 버퍼의 구조체로 설정하여 내부의 값을 수정 할 수 있게 한다.
	VertexBufferDesc.ByteWidth = sizeof(BitmapVertexType) * m_iVertexCount;
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 리소스 구조체 설정
	VertexData.pSysMem = vertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	hr = pDevice->CreateBuffer(&VertexBufferDesc, &VertexData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// 인덱스버퍼 구조체 초기화
	ZeroMemory(&IndexBufferDesc, sizeof(IndexBufferDesc));

	// 인덱스 버퍼의 구조체 설정
	// 인덱스 버퍼는 정점 위치가 바뀌어도 항상 같은 6개의 정점을 가리키기 때문에
	// 인덱스 버퍼를 동적 버퍼로 만들 필요는 없다.
	IndexBufferDesc.ByteWidth = sizeof(ULONG) * m_iIndexCount;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 리소스 구조체 설정
	IndexData.pSysMem = indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	hr = pDevice->CreateBuffer(&IndexBufferDesc, &IndexData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// 정점 버퍼와 인덱스 버퍼를 만든 후에는 이미 값이 복사되어 필요 없어진 정점 배열과
	// 인덱스 배열을 제거 할 수 있다.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;

}

bool BitmapClass::Render(ID3D11DeviceContext* pContext, int PosX, int PosY)
{
	bool result;

	result = UpdateBuffer(pContext, PosX, PosY);
	if (!result)
	{
		return false;
	}

	// 정점버퍼와 인덱스버퍼를 그래픽스 파이프라인에 보내 화면에 랜더링 할 준비
	RenderBuffer(pContext);

	return true;
}

bool BitmapClass::RenderBuffer(ID3D11DeviceContext* pContext)
{
	UINT stride;
	UINT OffSet;

	// 정점 버퍼의 단위와 오프셋 설정
	stride = sizeof(BitmapVertexType);
	OffSet = 0;

	// input assembler에 정점 버퍼를 활성화하여 렌더링 할 수 있게 한다
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &OffSet);

	// input assembler에 인덱스 버퍼를 활성화하여 렌더링 할 수 있게 한다
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool BitmapClass::Release()
{
	ReleaseModelText();
	ReleaseTexture();

	// 정점버퍼와 인덱스버퍼 해제
	ReleaseBuffer();

	return true;
}

bool BitmapClass::ReleaseModelText()
{
	//if (m_pModelType)
	//{
	//	delete[] m_pModelType;
	//	m_pModelType = NULL;
	//}

	return true;
}

bool BitmapClass::ReleaseTexture()
{
	if (m_pTextureClass)
	{
		m_pTextureClass->Release();
		delete m_pTextureClass;
		m_pTextureClass = 0;
	}

	return true;
}

bool BitmapClass::ReleaseBuffer()
{
	// 인덱스 버퍼 해제
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}
	// 버텍스 버퍼 해제
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = NULL;
	}
	if (m_pTextureClass)
	{
		m_pTextureClass->Release();
		m_pTextureClass = NULL;
	}

	return true;
}

BitmapClass::~BitmapClass()
{

}
