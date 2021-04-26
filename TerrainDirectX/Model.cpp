#include "Model.h"

Model::Model()
{
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

// ���� �ε��� ������ �˷��ִ� �Լ�
int Model::GetIndexCount()
{
	return m_iIndexCount;
}

bool Model::Init()
{
	bool result;

	// �������ۿ� �ε������� �ʱ�ȭ
	// result = InitBuffers();

	return true;
}

bool Model::InitBuffers(ID3D11Device* pDevice)
{
	HRESULT hr;
	VertexType* vertices;
	ULONG* indices;
	D3D11_BUFFER_DESC VertexBufferDesc, IndexBufferDesc;
	D3D11_SUBRESOURCE_DATA VertexData, IndexData;

	// ���� �迭�� ũ�� ����
	m_iVertexCount = 3;
	
	// �ε��� �迭�� ũ�� ����
	m_iIndexCount = 3;

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

	// ���� / �ε��� �迭�� �ﰢ���� �� ���� ������ ����.
	// ������ �ð�������� ������ ��. �ݽð�������� ����� �Ǹ� DirectX���� �ﰢ���� �ݴ�����
	// �ٶ󺻴ٰ� �Ǵ��� Backface Culling�� ���� �׷����� �ʰԵ�.

	// ���� �迭�� �� ����
	vertices[0].pos = Vector3(-1.0f, -1.0f, 0.0f); // ���� �Ʒ�
	vertices[0].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].pos = Vector3(0.0f, 1.0f, 0.0f); // ��
	vertices[1].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].pos = Vector3(1.0f, -1.0f, 0.0f); // ������ �Ʒ�
	vertices[2].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);


	// �ε��� �迭�� �� ����
	indices[0] = 0; // ���� �Ʒ�
	indices[1] = 1; // ��
	indices[2] = 2; // ������ �Ʒ�

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
	ReleaseBuffer();

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