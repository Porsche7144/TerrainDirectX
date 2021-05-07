#include "TextureShader.h"

void TextureShader::OutputShaderErrorMessage(ID3DBlob* ErrorMessage, HWND hwnd, const WCHAR* filename)
{
	char* compileError;
	ULONG BufferSize, i;
	std::ofstream fout;

	// 에러 메세지를 담고 있는 문자열 버퍼 포인터를 가져옴
	compileError = (char*)ErrorMessage->GetBufferPointer();

	// 메세지 길이
	BufferSize = ErrorMessage->GetBufferSize();

	// 파일을 열고 안에 메세지 기록
	fout.open("Shader-Error.txt");

	// 에러 메세지 작성
	for (int i = 0; i < BufferSize; i++)
	{
		fout << compileError[i];
	}

	// 파일 Close
	fout.close();

	// 에러 메세지 메모리 반환
	ErrorMessage->Release();
	ErrorMessage = 0;

	// 컴파일 에러를 팝업 메세지로 표시
	MessageBox(hwnd, L"쉐이더 컴파일 에러. Shader-Error.txt 참고.", filename, MB_OK);

}

bool TextureShader::Init(ID3D11Device* pDevice, HWND hwnd)
{
	// 버텍스 쉐이더와 픽셀 쉐이더 초기화
	if (!InitShader(pDevice, hwnd, L"../Shader/TextureShaderVS.txt", L"../Shader/TextureShaderPS.txt"))
	{
		return false;
	}

	return true;
}

bool TextureShader::InitShader(ID3D11Device* pDevice, HWND hwnd, const WCHAR* vs, const WCHAR* ps)
{
	HRESULT hr;
	ID3DBlob* errorMessage = nullptr;

	// 쉐이더 파일을 불러와 DirectX와 GPU에서 사용 가능하도록 작업.
	ID3DBlob* VertexShaderBuffer = nullptr;
	ID3DBlob* PixelShaderBuffer = nullptr;
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// 버텍스 쉐이더 컴파일
	// 쉐이더 버전은 DirectX 11 에서는 5.0
	hr = D3DCompileFromFile(vs, NULL, NULL, "VS_VertexShader", "vs_5_0", 0, 0, &VertexShaderBuffer, &errorMessage);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vs);
		}
		else
		{
			MessageBox(hwnd, vs, L"VS 로드 실패. 파일을 찾지 못하였습니다.", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 컴파일
	hr = D3DCompileFromFile(ps, NULL, NULL, "PS_PixelShader", "ps_5_0", 0, 0, &PixelShaderBuffer, &errorMessage);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, ps);
		}
		else
		{
			MessageBox(hwnd, ps, L"PS 로드 실패. 파일을 찾지 못하였습니다.", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 버텍스 쉐이더 생성
	hr = pDevice->CreateVertexShader(VertexShaderBuffer->GetBufferPointer(), VertexShaderBuffer->GetBufferSize(),
		NULL, &m_pVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	// 버퍼로부터 픽셀 쉐이더 생성
	hr = pDevice->CreatePixelShader(PixelShaderBuffer->GetBufferPointer(), PixelShaderBuffer->GetBufferSize(),
		NULL, &m_pPixelShader);
	if (FAILED(hr))
	{
		return false;
	}


	// 쉐이더에서 사용할 정점 데이터의 레이아웃 구조체 생성
	// Model 클래스의 쉐이더의 VertexType 구조와 일치해야 한다.
	// LPCSTR SemanticName;				쉐이더에 있는 이름과 일치해야함.
	// UINT SemanticIndex;				
	// DXGI_FORMAT Format;				위치백터는 DXGI_FORMAT_R32G32B32_FLOAT
	//									색상백터는 DXGI_FORMAT_R32G32B32A32_FLOAT
	// UINT InputSlot;
	// UINT AlignedByteOffset;			버퍼에 데이터가 어떻게 배열될지, 각 요소가 어디서 시작하는지
	//									직접 입력도 가능하나, D3D11_APPEND_ALIGNED_ELEMENT를 
	//									사용해 자동으로 알아낼 수 있음.
	// D3D11_INPUT_CLASSIFICATION InputSlotClass;
	// UINT InstanceDataStepRate;
	const D3D11_INPUT_ELEMENT_DESC PolygonLayOut[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 레이아웃 요소의 개수
	numElements = sizeof(PolygonLayOut) / sizeof(PolygonLayOut[0]);

	// 정점 레이아웃 생성
	hr = pDevice->CreateInputLayout(PolygonLayOut, numElements, VertexShaderBuffer->GetBufferPointer(),
		VertexShaderBuffer->GetBufferSize(), &m_pLayOut);
	if (FAILED(hr))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼 해제
	VertexShaderBuffer->Release();
	VertexShaderBuffer = 0;

	PixelShaderBuffer->Release();
	PixelShaderBuffer = 0;


	// 상수버퍼 세팅
	// 상수버퍼는 정점 및 픽셀 쉐이더에서 사용될 상수를 모아 놓은 버퍼
	// 상수버퍼 사용을 위해 코드 영역에 상수 버퍼 타입의 구조체를 정의하고,
	// 쉐이더에서도 동일한 포맷으로 상수 버퍼 구조체를 정의
	// 그 후, 시스템 메모리에서 구조체 변수 생성 및 값 설정 후 정점 혹은 픽셀 쉐이더에 Set

	// 정점 쉐이더에 있는 행렬 상수 버퍼 구조체 작성
	// 매 프레임마다 상수버퍼를 업데이트 하므로 BindFlags를 상수버퍼로 바인드
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 생성하여 정점 쉐이더 상수 버퍼에 접근 할 수 있도록 함
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		return false;
	}


	// 텍스쳐 샘플러 상태 구조체 설정
	D3D11_SAMPLER_DESC SamplerDesc;

	// Filter 필터링 방식
	// 최종 도형 표면에서 텍스쳐의 어느 픽셀이 사용되거나 혼합될 것인지 결정
	SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// 텍스쳐 주소 지정방식
	SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 16;
	SamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamplerDesc.BorderColor[0] = 1.0f;
	SamplerDesc.BorderColor[1] = 0;
	SamplerDesc.BorderColor[2] = 0;
	SamplerDesc.BorderColor[3] = 1.0f;
	SamplerDesc.MinLOD = FLT_MIN;
	SamplerDesc.MaxLOD = FLT_MAX;

	// 텍스쳐 샘플러 스테이트 생성
	hr = pDevice->CreateSamplerState(&SamplerDesc, &m_pSamplerState);
	if (FAILED(hr))
	{
		return false;
	}


	return true;
}

bool TextureShader::Render(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* texture, int IndexCount, Matrix World, Matrix View, Matrix Proj)
{

	// 렌더링에 사용할 쉐이더 인자 입력
	if (!SetShaderParameters(pContext, texture, World, View, Proj))
	{
		return false;
	}

	// 쉐이더를 이용해 준비된 버퍼를 렌더링
	RenderShader(pContext, IndexCount);

	return true;
}

bool TextureShader::RenderShader(ID3D11DeviceContext* pContext, int IndexCount)
{

	// 정점 입력 레이아웃 설정
	pContext->IASetInputLayout(m_pLayOut);

	// 삼각형을 그릴 정점 쉐이더와 픽셀 쉐이더 설정
	pContext->VSSetShader(m_pVertexShader, NULL, 0);
	pContext->PSSetShader(m_pPixelShader, NULL, 0);

	// 픽셀 쉐이더에서 샘플러 상태 설정
	// pContext->VSSetSamplers(0, 1, &m_pSamplerState);
	pContext->PSSetSamplers(0, 1, &m_pSamplerState);

	// 삼각형 렌더링
	pContext->DrawIndexed(IndexCount, 0, 0);

	return true;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* pContext, ID3D11ShaderResourceView* texture, Matrix World, Matrix View, Matrix Proj)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	MatrixBufferType* pData;
	UINT BufferNumber;

	// 행렬을 전치하여 쉐이더에서 사용 할 수 있게 함.
	World = World.Transpose();
	View = View.Transpose();
	Proj = Proj.Transpose();

	// 상수 버퍼의 내용을 쓸 수 있도록 잠금
	hr = pContext->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		// 상수 버퍼의 데이터에 대한 포인터를 가져옴
		pData = (MatrixBufferType*)MappedResource.pData;

		// 상수 버퍼에 행렬을 복사
		pData->world = World;
		pData->view = View;
		pData->Projection = Proj;
	}


	// 상수버퍼 잠금 해제
	pContext->Unmap(m_pConstantBuffer, 0);

	// 정점 쉐이더에서의 상수 버퍼의 위치 설정
	BufferNumber = 0;

	// 정점 쉐이더의 상수 버퍼를 바뀐 값으로 설정
	pContext->VSSetConstantBuffers(BufferNumber, 1, &m_pConstantBuffer);

	// 픽셀 쉐이더에서 쉐이더 텍스쳐 리소스 설정
	pContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

bool TextureShader::Release()
{
	ReleaseShader();

	return true;
}

bool TextureShader::ReleaseShader()
{
	// 텍스쳐 샘플러 해제
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();
		m_pSamplerState = 0;
	}

	// 상수 버퍼 해제
	if (m_pConstantBuffer)
	{
		m_pConstantBuffer->Release();
		m_pConstantBuffer = 0;
	}

	// 레이아웃 해제
	if (m_pLayOut)
	{
		m_pLayOut->Release();
		m_pLayOut = 0;
	}

	// 정점 쉐이더 해제
	if (m_pVertexShader)
	{
		m_pVertexShader->Release();
		m_pVertexShader = 0;
	}

	// 픽셀 쉐이더 해제
	if (m_pPixelShader)
	{
		m_pPixelShader->Release();
		m_pPixelShader = 0;
	}

	return true;
}

TextureShader::TextureShader()
{
	m_pVertexShader = nullptr;
	m_pPixelShader = nullptr;
	m_pLayOut = nullptr;
	m_pConstantBuffer = nullptr;
	m_pSamplerState = nullptr;
}

TextureShader::~TextureShader()
{

}
