#include "BitmapClass.h"

BitmapClass::BitmapClass()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTextureClass = nullptr;
	// m_pModelType = nullptr;
}

// ���� �ε��� ������ �˷��ִ� �Լ�
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
	// �ؽ��� ������Ʈ ����
	m_pTextureClass = new Texture;
	if (!m_pTextureClass)
	{
		return false;
	}

	// �ؽ��� ������Ʈ �ʱ�ȭ
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

	// �ؽ�Ʈ ���� ����
	fin.open(texName);

	// ���� ���⿡ �����ϸ� ������
	if (fin.fail())
	{
		return false;
	}

	// ���� ī��Ʈ�� ���� �б�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// ���� ī��Ʈ ������ �д´�
	fin >> m_iVertexCount;

	// �ε��� ���� ���� ���� ���� ���� ����
	m_iIndexCount = m_iVertexCount;

	// �о�� ���� ������ŭ ��ü ����
	//m_pModelType = new ModelType[m_iVertexCount];
	//if (!m_pModelType)
	//{
	//	return false;
	//}

	// ������ ���ۺκ� ���� �б�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���� ������ �о�´�.
	//for (int i = 0; i < m_iVertexCount; i++)
	//{
	//	fin >> m_pModelType[i].x >> m_pModelType[i].y >> m_pModelType[i].z;
	//	fin >> m_pModelType[i].tu >> m_pModelType[i].tv;
	//	fin >> m_pModelType[i].nx >> m_pModelType[i].ny >> m_pModelType[i].nz;
	//}

	// �ؽ�Ʈ �ݱ�
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

	// ��Ʈ�� ������ �� ��ġ�� ������� ���� ��� �������۸� ������Ʈ ���� �ʰ� ��ȯ
	if (PosX == m_PrevPosX && PosY == m_PrevPosY)
	{
		return true;
	}
	// �׷��� �ʴٸ� ������ ��ġ ������Ʈ
	else
	{
		m_PrevPosX = PosX;
		m_PrevPosY = PosY;

		// ��Ʈ�� ȭ����ǥ ���
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

	// ���� �迭 ������ �ε�  ù��° �ﰢ��
	vertices[0].pos = Vector3(left, top, 0.0f);
	vertices[0].TextureUV = Vector2(0.0f, 0.0f);

	vertices[1].pos = Vector3(right, bottom, 0.0f);
	vertices[1].TextureUV = Vector2(1.0f, 1.0f);

	vertices[2].pos = Vector3(left, bottom, 0.0f);
	vertices[2].TextureUV = Vector2(0.0f, 1.0f);

	// �ι�° �ﰢ��
	vertices[3].pos = Vector3(left, top, 0.0f);
	vertices[3].TextureUV = Vector2(0.0f, 0.0f);

	vertices[4].pos = Vector3(right, top, 0.0f);
	vertices[4].TextureUV = Vector2(1.0f, 0.0f);

	vertices[5].pos = Vector3(right, bottom, 0.0f);
	vertices[5].TextureUV = Vector2(1.0f, 1.0f);

	// ���ؽ� ���۸� ���� �����ϵ��� ���
	hr = pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(hr))
	{
		return false;
	}
	else
	{
		// ���ؽ� ������ �����Ϳ� ���� �����͸� ������
		verticesPtr = (BitmapVertexType*)mappedResource.pData;
		// �����͸� ���� ���ۿ� ����
		memcpy(verticesPtr, (void*)vertices, (sizeof(BitmapVertexType) * m_iVertexCount));
	}

	pContext->Unmap(m_pVertexBuffer, 0);

	// ������ۿ� �ѱ� �� �ʿ����� ���� ������ ����
	delete[] vertices;
	vertices = 0;

	return true;
}

bool BitmapClass::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename,
						int ScreenWidht, int ScreenHeight, int bitmapWidth, int bitmapHeight)
{
	bool result;
	
	// ��ũ���� ��Ʈ�� ������ ����
	m_ScreenHeight = ScreenHeight;
	m_ScreenWidth = ScreenWidht;

	// �������� ��Ʈ�� �ȼ�����
	m_BitmapHeight = bitmapHeight;
	m_BitmapWidth = bitmapWidth;

	// ���� ������ ��ġ�� ���̳ʽ��� �ʱ�ȭ. ���� ��ġ�� ����ϴ� �뵵.
	m_PrevPosX = -1;
	m_PrevPosY = -1;


	// �������ۿ� �ε������� �ʱ�ȭ
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

	// ���ؽ� �迭 ����
	vertices = new BitmapVertexType[m_iVertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭 ����
	indices = new ULONG[m_iIndexCount];
	if (!indices)
	{
		return false;
	}

	// ���� �迭 0���� �ʱ�ȭ
	memset(vertices, 0, sizeof(BitmapVertexType) * m_iVertexCount);

	// �ε��� �迭�� �� �Ҵ�
	for (int i = 0; i < m_iIndexCount; i++)
	{
		indices[i] = i;
	}

	// �������� ����ü �ʱ�ȭ
	ZeroMemory(&VertexBufferDesc, sizeof(VertexBufferDesc));

	// ���� ���� ������ ����ü�� �����Ͽ� ������ ���� ���� �� �� �ְ� �Ѵ�.
	VertexBufferDesc.ByteWidth = sizeof(BitmapVertexType) * m_iVertexCount;
	VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���ҽ� ����ü ����
	VertexData.pSysMem = vertices;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	hr = pDevice->CreateBuffer(&VertexBufferDesc, &VertexData, &m_pVertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// �ε������� ����ü �ʱ�ȭ
	ZeroMemory(&IndexBufferDesc, sizeof(IndexBufferDesc));

	// �ε��� ������ ����ü ����
	// �ε��� ���۴� ���� ��ġ�� �ٲ� �׻� ���� 6���� ������ ����Ű�� ������
	// �ε��� ���۸� ���� ���۷� ���� �ʿ�� ����.
	IndexBufferDesc.ByteWidth = sizeof(ULONG) * m_iIndexCount;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;
	IndexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���ҽ� ����ü ����
	IndexData.pSysMem = indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	hr = pDevice->CreateBuffer(&IndexBufferDesc, &IndexData, &m_pIndexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// ���� ���ۿ� �ε��� ���۸� ���� �Ŀ��� �̹� ���� ����Ǿ� �ʿ� ������ ���� �迭��
	// �ε��� �迭�� ���� �� �� �ִ�.
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

	// �������ۿ� �ε������۸� �׷��Ƚ� ���������ο� ���� ȭ�鿡 ������ �� �غ�
	RenderBuffer(pContext);

	return true;
}

bool BitmapClass::RenderBuffer(ID3D11DeviceContext* pContext)
{
	UINT stride;
	UINT OffSet;

	// ���� ������ ������ ������ ����
	stride = sizeof(BitmapVertexType);
	OffSet = 0;

	// input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� ������ �� �� �ְ� �Ѵ�
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &OffSet);

	// input assembler�� �ε��� ���۸� Ȱ��ȭ�Ͽ� ������ �� �� �ְ� �Ѵ�
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ����
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool BitmapClass::Release()
{
	ReleaseModelText();
	ReleaseTexture();

	// �������ۿ� �ε������� ����
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
	// �ε��� ���� ����
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = NULL;
	}
	// ���ؽ� ���� ����
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
