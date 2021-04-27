#include "ColorShader.h"

void ColorShader::OutputShaderErrorMessage(ID3DBlob* ErrorMessage, HWND hwnd, const WCHAR* filename)
{
	char* compileError;
	ULONG BufferSize, i;
	std::ofstream fout;

	// ���� �޼����� ��� �ִ� ���ڿ� ���� �����͸� ������
	compileError = (char*)ErrorMessage->GetBufferPointer();
	
	// �޼��� ����
	BufferSize = ErrorMessage->GetBufferSize();

	// ������ ���� �ȿ� �޼��� ���
	fout.open("Shader-Error.txt");

	// ���� �޼��� �ۼ�
	for (int i = 0; i < BufferSize; i++)
	{
		fout << compileError[i];
	}

	// ���� Close
	fout.close();

	// ���� �޼��� �޸� ��ȯ
	ErrorMessage->Release();
	ErrorMessage = 0;

	// ������ ������ �˾� �޼����� ǥ��
	MessageBox(hwnd, L"���̴� ������ ����. Shader-Error.txt ����.", filename, MB_OK);

}

bool ColorShader::Init(ID3D11Device* pDevice, HWND hwnd)
{
	bool result;

	// ���ؽ� ���̴��� �ȼ� ���̴� �ʱ�ȭ
	result = InitShader(pDevice, hwnd, L"../Shader/colorVS.txt", L"../Shader/colorPS.txt");
	if (!result)
	{
		return false;
	}

	return true;
}

bool ColorShader::InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps)
{
	HRESULT hr;
	ID3DBlob* errorMessage = nullptr;
	
	// ���̴� ������ �ҷ��� DirectX�� GPU���� ��� �����ϵ��� �۾�.
	ID3DBlob* VertexShaderBuffer = nullptr;
	ID3DBlob* PixelShaderBuffer = nullptr;
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// ���ؽ� ���̴� ������
	// ���̴� ������ DirectX 11 ������ 5.0
	hr = D3DCompileFromFile(vs, NULL, NULL, "ColorVertexShader", "vs_5_0", 0, 0, &VertexShaderBuffer, &errorMessage);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vs);
		}
		else
		{
			MessageBox(hwnd, vs, L"VS �ε� ����. ������ ã�� ���Ͽ����ϴ�.", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� ������
	hr = D3DCompileFromFile(ps, NULL, NULL, "ColorPixelShader", "ps_5_0", 0, 0, &PixelShaderBuffer, &errorMessage);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, ps);
		}
		else
		{
			MessageBox(hwnd, ps, L"PS �ε� ����. ������ ã�� ���Ͽ����ϴ�.", MB_OK);
		}

		return false;
	}

	// ���۷κ��� ���ؽ� ���̴� ����
	hr = pDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(), 
									 NULL, &m_pVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	// ���۷κ��� �ȼ� ���̴� ����
	hr = pDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(), 
									NULL, &m_pPixelShader);
	if (FAILED(hr))
	{
		return false;
	}


	// ���̴����� ����� ���� �������� ���̾ƿ� ����
	// LPCSTR SemanticName;				���̴��� �ִ� �̸��� ��ġ�ؾ���.
	// UINT SemanticIndex;				
	// DXGI_FORMAT Format;				��ġ���ʹ� DXGI_FORMAT_R32G32B32_FLOAT
	//									������ʹ� DXGI_FORMAT_R32G32B32A32_FLOAT
	// UINT InputSlot;
	// UINT AlignedByteOffset;			���ۿ� �����Ͱ� ��� �迭����, �� ��Ұ� ��� �����ϴ���
	//									���� �Էµ� �����ϳ�, D3D11_APPEND_ALIGNED_ELEMENT�� 
	//									����� �ڵ����� �˾Ƴ� �� ����.
	// D3D11_INPUT_CLASSIFICATION InputSlotClass;
	// UINT InstanceDataStepRate;
	const D3D11_INPUT_ELEMENT_DESC PolygonLayOut[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// ���̾ƿ� ����� ����
	numElements = sizeof(PolygonLayOut) / sizeof(PolygonLayOut[1]);

	// ���� ���̾ƿ� ����
	hr = pDevice->CreateInputLayout(PolygonLayOut, numElements, VertexShaderBuffer->GetBufferPointer(), 
									VertexShaderBuffer->GetBufferSize(), &m_pLayOut);
	if (FAILED(hr))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���� ����
	VertexShaderBuffer->Release();
	VertexShaderBuffer = 0;

	PixelShaderBuffer->Release();
	PixelShaderBuffer = 0;


	// ������� ����
	// ������۴� ���� �� �ȼ� ���̴����� ���� ����� ��� ���� ����
	// ������� ����� ���� �ڵ� ������ ��� ���� Ÿ���� ����ü�� �����ϰ�,
	// ���̴������� ������ �������� ��� ���� ����ü�� ����
	// �� ��, �ý��� �޸𸮿��� ����ü ���� ���� �� �� ���� �� ���� Ȥ�� �ȼ� ���̴��� Set

	// ���� ���̴��� �ִ� ��� ��� ���� ����ü �ۼ�
	// �� �����Ӹ��� ������۸� ������Ʈ �ϹǷ� BindFlags�� ������۷� ���ε�
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����Ͽ� ���� ���̴� ��� ���ۿ� ���� �� �� �ֵ��� ��
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}

bool ColorShader::Render(ID3D11DeviceContext* pContext, int IndexCount, Matrix World, Matrix View, Matrix Proj)
{
	bool result;

	// �������� ����� ���̴� ���� �Է�
	result = SetShaderParameters(pContext, World, View, Proj);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ColorShader::RenderShader(ID3D11DeviceContext* pContext, int IndexCount)
{

	// ���� �Է� ���̾ƿ� ����
	pContext->IASetInputLayout(m_pLayOut);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴� ����
	pContext->VSSetShader(m_pVertexShader, NULL, 0);
	pContext->PSSetShader(m_pPixelShader, NULL, 0);

	// �ﰢ�� ������
	pContext->DrawIndexed(IndexCount, 0, 0);

	return true;
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* pContext, Matrix World, Matrix View, Matrix Proj)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	MatrixBufferType* pData;
	UINT BufferNumber;

	// ����� ��ġ�Ͽ� ���̴����� ��� �� �� �ְ� ��.
	World = World.Transpose();
	View = View.Transpose();
	Proj = Proj.Transpose();

	// ��� ������ ������ �� �� �ֵ��� ���
	hr = pContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		// ��� ������ �����Ϳ� ���� �����͸� ������
		pData = (MatrixBufferType*)MappedResource.pData;

		// ��� ���ۿ� ����� ����
		pData->world = World;
		pData->view = View;
		pData->Projection = Proj;
	}
	// ������� ��� ����
	pContext->Unmap(m_pMatrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ ����
	BufferNumber = 0;

	// ���� ���̴��� ��� ���۸� �ٲ� ������ ����
	pContext->VSSetConstantBuffers(BufferNumber, 1, &m_pMatrixBuffer);


	return true;
}

bool ColorShader::Release()
{
	ReleaseShader();

	return true;
}

bool ColorShader::ReleaseShader()
{
	// ��� ���� ����
	if (m_pMatrixBuffer)
	{
		m_pMatrixBuffer->Release();
		m_pMatrixBuffer = 0;
	}

	// ���̾ƿ� ����
	if (m_pLayOut)
	{
		m_pLayOut->Release();
		m_pLayOut = 0;
	}

	// ���� ���̴� ����
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = 0;
	}

	// �ȼ� ���̴� ����
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = 0;
	}

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
