#include "Model.h"

Model::Model()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
	m_pTextureClass = nullptr;
	m_pModelType = nullptr;
}

// ���� �ε��� ������ �˷��ִ� �Լ�
int Model::GetIndexCount()
{
	return m_iIndexCount;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_pTextureClass->GetTexture();
}

bool Model::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename)
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

bool Model::LoadModelText(const TCHAR* texName)
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
	m_pModelType = new ModelType[m_iVertexCount];
	if (!m_pModelType)
	{
		return false;
	}

	// ������ ���ۺκ� ���� �б�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// ���� ������ �о�´�.
	for (int i = 0; i < m_iVertexCount; i++)
	{
		fin >> m_pModelType[i].x >> m_pModelType[i].y >> m_pModelType[i].z;
		fin >> m_pModelType[i].tu >>  m_pModelType[i].tv;
		fin >> m_pModelType[i].nx >>  m_pModelType[i].ny >> m_pModelType[i].nz;
	}
	
	// �ؽ�Ʈ �ݱ�
	fin.close();

	return true;
}

bool Model::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename, const TCHAR* texName)
{
	bool result;


	// �ؽ�Ʈ �ε�
	result = LoadModelText(texName);
	if (!result)
	{
		return false;
	}

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

bool Model::InitBuffers(ID3D11Device* pDevice)
{
	HRESULT hr;
	VertexType* vertices;
	ULONG* indices;
	D3D11_BUFFER_DESC VertexBufferDesc, IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA VertexData, IndexData;

	// ���ؽ� ����
	vertices = new VertexType[m_iVertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� ����
	indices = new ULONG[m_iIndexCount];
	if (!indices)
	{
		return false;
	}


	// ���Ϸ� �о�� ���� / �ε��� �����͸�
	// ���� / �ε��� �迭�� �ﰢ���� �� ���� ������ ����.
	// ������ �ð�������� ������ ��. �ݽð�������� ����� �Ǹ� DirectX���� �ﰢ���� �ݴ�����
	// �ٶ󺻴ٰ� �Ǵ��� Backface Culling�� ���� �׷����� �ʰԵ�.
	for (int i = 0; i < m_iVertexCount; i++)
	{
		vertices[i].pos = Vector3(m_pModelType[i].x, m_pModelType[i].y, m_pModelType[i].z);
		vertices[i].TextureUV = Vector2(m_pModelType[i].tu, m_pModelType[i].tv);
		vertices[i].normal = Vector3(m_pModelType[i].nx, m_pModelType[i].ny, m_pModelType[i].nz);

		indices[i] = i;
	}

	// �������� ����ü �ʱ�ȭ
	ZeroMemory(&VertexBufferDesc, sizeof(VertexBufferDesc));

	// ���� ������ ����ü ����
	VertexBufferDesc.ByteWidth = sizeof(VertexType) * m_iVertexCount;
	VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = 0;
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

bool Model::Render(ID3D11DeviceContext* pContext)
{
	// �������ۿ� �ε������۸� �׷��Ƚ� ���������ο� ���� ȭ�鿡 ������ �� �غ�
	RenderBuffer(pContext);

	return true;
}

bool Model::RenderBuffer(ID3D11DeviceContext* pContext)
{
	UINT stride;
	UINT OffSet;

	// ���� ������ ������ ������ ����
	stride = sizeof(VertexType);
	OffSet = 0;

	// input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� ������ �� �� �ְ� �Ѵ�
	pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &OffSet);

	// input assembler�� �ε��� ���۸� Ȱ��ȭ�Ͽ� ������ �� �� �ְ� �Ѵ�
	pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻�� ����
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool Model::Release()
{
	ReleaseModelText();
	ReleaseTexture();

	// �������ۿ� �ε������� ����
	ReleaseBuffer();

	return true;
}

bool Model::ReleaseModelText()
{
	if (m_pModelType)
	{
		delete[] m_pModelType;
		m_pModelType = NULL;
	}

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

	return true;
}

Model::~Model()
{

}
