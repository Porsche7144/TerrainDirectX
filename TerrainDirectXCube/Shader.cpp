#include "Shader.h"

void Shader::OutputShaderErrorMessage(ID3DBlob* ErrorMessage, HWND hwnd, const WCHAR* filename)
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

bool Shader::Init(ID3D11Device* pDevice, HWND hwnd)
{
	// ���ؽ� ���̴��� �ȼ� ���̴� �ʱ�ȭ
	if(!InitShader(pDevice, hwnd, L"../Shader/LightVS.txt", L"../Shader/LightPS.txt"))
	{
		return false;
	}

	return true;
}

bool Shader::InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps)
{
	HRESULT hr;
	ID3DBlob* errorMessage = nullptr;
	
	// ���̴� ������ �ҷ��� DirectX�� GPU���� ��� �����ϵ��� �۾�.
	ID3DBlob* VertexShaderBuffer = nullptr;
	ID3DBlob* PixelShaderBuffer = nullptr;
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;


	// ���ؽ� ���̴� ������
	// ���̴� ������ DirectX 11 ������ 5.0
	hr = D3DCompileFromFile(vs, NULL, NULL, "VS_VertexShader", "vs_5_0", 0, 0, &VertexShaderBuffer, &errorMessage);
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
	hr = D3DCompileFromFile(ps, NULL, NULL, "PS_PixelShader", "ps_5_0", 0, 0, &PixelShaderBuffer, &errorMessage);
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


	// ���̴����� ����� ���� �������� ���̾ƿ� ����ü ����
	// Model Ŭ������ ���̴��� VertexType ������ ��ġ�ؾ� �Ѵ�.
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
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// �븻������ x,y,z ������ ���� �� �ִ� DXGI_FORMAT_R32G32B32_FLOAT�� ����
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// ���̾ƿ� ����� ����
	numElements = sizeof(PolygonLayOut) / sizeof(PolygonLayOut[0]);

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


	// �ؽ��� ���÷� ���� ����ü ����
	D3D11_SAMPLER_DESC SamplerDesc;

	// Filter ���͸� ���
	// ���� ���� ǥ�鿡�� �ؽ����� ��� �ȼ��� ���ǰų� ȥ�յ� ������ ����
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// �ؽ��� �ּ� �������
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerDesc.BorderColor[0] = 0;
	SamplerDesc.BorderColor[1] = 0;
	SamplerDesc.BorderColor[2] = 0;
	SamplerDesc.BorderColor[3] = 0;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �ؽ��� ���÷� ������Ʈ ����
	hr = pDevice->CreateSamplerState(&SamplerDesc, &m_pSamplerState);
	if (FAILED(hr))
	{
		return false;
	}

	// ������� ����
	// ������۴� ���� �� �ȼ� ���̴����� ���� ����� ��� ���� ����
	// ������� ����� ���� �ڵ� ������ ��� ���� Ÿ���� ����ü�� �����ϰ�,
	// ���̴������� ������ �������� ��� ���� ����ü�� ����
	// �� ��, �ý��� �޸𸮿��� ����ü ���� ���� �� �� ���� �� ���� Ȥ�� �ȼ� ���̴��� Set

	// ���� ���̴��� �ִ� ��� ��� ���� ����ü �ۼ�
	// �� �����Ӹ��� ������۸� ������Ʈ �ϹǷ� BindFlags�� ������۷� ���ε�
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����Ͽ� ���� ���̴� ��� ���ۿ� ���� �� �� �ֵ��� ��
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// ī�޶� ������� ����
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// ī�޶� ��� ���� �����Ͽ� ���� ���̴� ��� ���ۿ� ���� �� �� �ֵ��� ��
	hr = pDevice->CreateBuffer(&cameraBufferDesc, NULL, &m_pCameraConstantBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// ����Ʈ ������� ����
	// ��� ������ ũ�Ⱑ 16�� ������� �����ؾ��Ѵ�.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// ����Ʈ ��� ���� �����Ͽ� ���� ���̴� ��� ���ۿ� ���� �� �� �ֵ��� ��
	hr = pDevice->CreateBuffer(&lightBufferDesc, NULL, &m_pLightConstantBuffer);
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}

bool Shader::Render(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* texture, int IndexCount, 
					Matrix World, Matrix View, Matrix Proj,
					Vector3 LightDir, Vector4 DiffuseColor, Vector4 AmbientColor, Vector3 CameraPos, 
					float SpecularPower, Vector4 SpecularColor)
{

	// �������� ����� ���̴� ���� �Է�
	if(!SetShaderParameters(pContext, texture, World, View, Proj, LightDir, DiffuseColor , AmbientColor, 
						    CameraPos, SpecularPower, SpecularColor))
	{
		return false;
	}

	// ���̴��� �̿��� �غ�� ���۸� ������
	RenderShader(pContext, IndexCount);

	return true;
}

bool Shader::RenderShader(ID3D11DeviceContext* pContext, int IndexCount)
{

	// ���� �Է� ���̾ƿ� ����
	pContext->IASetInputLayout(m_pLayOut);
	 
	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴� ����
	pContext->VSSetShader(m_pVertexShader, NULL, 0);
	pContext->PSSetShader(m_pPixelShader, NULL, 0);

	// �ȼ� ���̴����� ���÷� ���� ����
	// pContext->VSSetSamplers(0, 1, &m_pSamplerState);
	pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// �ﰢ�� ������
	pContext->DrawIndexed(IndexCount, 0, 0);

	return true;
}

bool Shader::SetShaderParameters(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* texture ,
								Matrix World, Matrix View, Matrix Proj, 
								Vector3 LightDir, Vector4 DiffuseColor, Vector4 AmbientColor, Vector3 CameraPos,
								float SpecularPower, Vector4 SpecularColor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	MatrixBufferType* pData;
	LightBufferType* pLightData;
	CameraBufferType* pCameraData;
	UINT BufferNumber;

	Matrix worldMat;
	Matrix viewMat;
	Matrix projMat;

	// ����� ��ġ�Ͽ� ���̴����� ��� �� �� �ְ� ��.
	worldMat = World.Transpose();
	viewMat = View.Transpose();
	projMat = Proj.Transpose();

	// ��� ������ ������ �� �� �ֵ��� ���
	hr = pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		// ��� ������ �����Ϳ� ���� �����͸� ������
		pData = (MatrixBufferType*)MappedResource.pData;

		// ��� ���ۿ� ����� ����
		pData->world = worldMat;
		pData->view = viewMat;
		pData->Projection = projMat;
	}

	// ������� ��� ����
	pContext->Unmap(m_pConstantBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ ����
	BufferNumber = 0;

	// ���� ���̴��� ��� ���۸� �ٲ� ������ ����
	pContext->VSSetConstantBuffers(BufferNumber, 1, &m_pConstantBuffer);



	// ī�޶� ������� ����
	// Map�Լ��� ���ۿ� Lock�� �ɰ� ������ �����͸� ���´�
	hr = pContext->Map(m_pCameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		pCameraData = (CameraBufferType*)MappedResource.pData;

		// ���ۿ� ����Ʈ ����� ���� ����
		pCameraData->CameraPos = CameraPos;
		pCameraData->Padding = 0.0f;
	}

	// ������� ��� ����
	pContext->Unmap(m_pCameraConstantBuffer, 0);

	// ���ؽ� ���̴������� ��� ������ ��ġ ����
	// ī�޶� ���۴� ���ؽ� ���̴����� �ι�° �����̹Ƿ� 1�� ����
	BufferNumber = 1;

	// ���ؽ� ���̴��� ���� ����
	// ���ؽ� ���̴��� �����ϴ� ���̱� ������ PSSetConstantBuffer �Լ� ���
	pContext->VSSetConstantBuffers(BufferNumber, 1, &m_pCameraConstantBuffer);



	// �ȼ� ���̴����� ���̴� �ؽ��� ���ҽ� ����
	pContext->PSSetShaderResources(0, 1, &texture);



	// ���� ������� ����
	// Map�Լ��� ���ۿ� Lock�� �ɰ� ������ �����͸� ���´�
	hr = pContext->Map(m_pLightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if(FAILED(hr))
	{
		return false;
	}
	else
	{
		pLightData = (LightBufferType*)MappedResource.pData;

		// ���ۿ� ����Ʈ ����� ���� ����
		pLightData->AmbientColor = AmbientColor;
		pLightData->DiffuseColor = DiffuseColor;
		pLightData->LightDirection = LightDir;
		pLightData->SpecularPower = SpecularPower;
		pLightData->SpecularColor = SpecularColor;
		// pLightData->Padding = 0.0f;
	}

	// ������� ��� ����
	pContext->Unmap(m_pLightConstantBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ ����
	BufferNumber = 0;

	// �ȼ� ���̴��� ���� ����
	// �ȼ� ���̴��� �����ϴ� ���̱� ������ PSSetConstantBuffer �Լ� ���
	pContext->PSSetConstantBuffers(BufferNumber, 1, &m_pLightConstantBuffer);



	return true;
}

bool Shader::Release()
{
	ReleaseShader();

	return true;
}

bool Shader::ReleaseShader()
{
	// �ؽ��� ���÷� ����
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = 0;
	}

	// ��� ���� ����
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = 0;
	}

	if (m_pCameraConstantBuffer)
	{
		m_pCameraConstantBuffer->Release();
		m_pCameraConstantBuffer = 0;
	}

	if (m_pLightConstantBuffer)
	{
		m_pLightConstantBuffer->Release();
		m_pLightConstantBuffer = 0;
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

Shader::Shader()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayOut = nullptr;
	m_pConstantBuffer = nullptr;
	m_pSamplerState = nullptr;
	m_pLightConstantBuffer = nullptr;
	m_pCameraConstantBuffer = nullptr;
}

Shader::~Shader()
{

}
