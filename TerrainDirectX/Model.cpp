#include "Model.h"

Model::Model()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTextureClass = nullptr;
}

// 모델의 인덱스 개수를 알려주는 함수
int Model::GetIndexCount()
{
	return m_iIndexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_pTextureClass->GetTexture();
}

bool Model::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string Filename)
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

bool Model::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string Filename)
{
	bool result;

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

bool Model::InitBuffers(ID3D11Device* pDevice)
{
	HRESULT hr;
	VertexType* vertices;
	ULONG* indices;
	D3D11_BUFFER_DESC VertexBufferDesc, IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA VertexData, IndexData;

	// 정점 배열의 크기 설정
	m_iVertexCount = 4;
	
	// 인덱스 배열의 크기 설정
	m_iIndexCount = 6;

	// 버텍스 생성
	vertices = new VertexType[m_iVertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 생성
	indices = new ULONG[m_iIndexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 / 인덱스 배열에 삼각형의 각 점과 순서를 저장.
	// 점들을 시계방향으로 만들어야 함. 반시계방향으로 만들게 되면 DirectX에서 삼각형이 반대편을
	// 바라본다고 판단해 Backface Culling에 의해 그려지지 않게됨.

	// 정점 배열에 값 저장
	vertices[0].pos = Vector3(-1.0f, 1.0f, 0.0f); // 왼쪽 아래
	vertices[0].TextureUV = Vector2(0.0f, 0.0f);

	vertices[1].pos = Vector3(1.0f, 1.0f, 0.0f); // 위 왼쪽
	vertices[1].TextureUV = Vector2(1.0f, 0.0f);

	vertices[2].pos = Vector3(1.0f, -1.0f, 0.0f); // 오른쪽 위
	vertices[2].TextureUV = Vector2(1.0f, 1.0f);

	vertices[3].pos = Vector3(-1.0f, -1.0f, 0.0f); // 오른쪽 아래
	vertices[3].TextureUV = Vector2(0.0f, 1.0f);


	// 인덱스 배열에 값 저장
	indices[0] = 0; // 왼쪽 아래
	indices[1] = 1; // 위
	indices[2] = 2; // 오른쪽 아래

	indices[3] = 0; // 위 왼쪽
	indices[4] = 2; // 위 오른쪽
	indices[5] = 3; // 오른쪽 아래

	// 정점버퍼 구조체 초기화
	ZeroMemory(&VertexBufferDesc, sizeof(VertexBufferDesc));

	// 정점 버퍼의 구조체 설정
	VertexBufferDesc.ByteWidth = sizeof(VertexType) * m_iVertexCount;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
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

	// 인덱스 버퍼의 구조체 설정
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

bool Model::Render(ID3D11DeviceContext* pContext)
{
	// 정점버퍼와 인덱스버퍼를 그래픽스 파이프라인에 보내 화면에 랜더링 할 준비
	RenderBuffer(pContext);

	return true;
}

bool Model::RenderBuffer(ID3D11DeviceContext* pContext)
{
	UINT stride;
	UINT OffSet;

	// 정점 버퍼의 단위와 오프셋 설정
	stride = sizeof(VertexType);
	OffSet = 0;

	// input assembler에 정점 버퍼를 활성화하여 렌더링 할 수 있게 한다
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &OffSet);

	// input assembler에 인덱스 버퍼를 활성화하여 렌더링 할 수 있게 한다
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형 설정
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool Model::Release()
{

	ReleaseTexture();

	// 정점버퍼와 인덱스버퍼 해제
	ReleaseBuffer();

	return true;
}

bool Model::ReleaseTexture()
{
	if (m_pTextureClass)
	{
		m_pTextureClass->Release();
		delete m_pTextureClass;
		m_pTextureClass = 0;
	}

	return true;
}

bool Model::ReleaseBuffer()
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

	return true;
}

Model::~Model()
{

}
