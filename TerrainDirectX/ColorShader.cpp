#include "ColorShader.h"

void ColorShader::OutputShaderErrorMessage(ID3DBlob* ErrorMessage, HWND hwnd, const WCHAR* filename)
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

bool ColorShader::Init(ID3D11Device* pDevice, HWND hwnd)
{
	bool result;

	// 버텍스 쉐이더와 픽셀 쉐이더 초기화
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
	
	// 쉐이더 파일을 불러와 DirectX와 GPU에서 사용 가능하도록 작업.
	ID3DBlob* VertexShaderBuffer = nullptr;
	ID3DBlob* PixelShaderBuffer = nullptr;
	UINT numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	// 버텍스 쉐이더 컴파일
	// 쉐이더 버전은 DirectX 11 에서는 5.0
	hr = D3DCompileFromFile(vs, NULL, NULL, "ColorVertexShader", "vs_5_0", 0, 0, &VertexShaderBuffer, &errorMessage);
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
	hr = D3DCompileFromFile(ps, NULL, NULL, "ColorPixelShader", "ps_5_0", 0, 0, &PixelShaderBuffer, &errorMessage);
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


	// 쉐이더에서 사용할 정점 데이터의 레이아웃 생성
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// 레이아웃 요소의 개수
	numElements = sizeof(PolygonLayOut) / sizeof(PolygonLayOut[1]);

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
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 생성하여 정점 쉐이더 상수 버퍼에 접근 할 수 있도록 함
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

	// 렌더링에 사용할 쉐이더 인자 입력
	result = SetShaderParameters(pContext, World, View, Proj);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ColorShader::RenderShader(ID3D11DeviceContext* pContext, int IndexCount)
{

	// 정점 입력 레이아웃 설정
	pContext->IASetInputLayout(m_pLayOut);

	// 삼각형을 그릴 정점 쉐이더와 픽셀 쉐이더 설정
	pContext->VSSetShader(m_pVertexShader, NULL, 0);
	pContext->PSSetShader(m_pPixelShader, NULL, 0);

	// 삼각형 렌더링
	pContext->DrawIndexed(IndexCount, 0, 0);

	return true;
}

bool ColorShader::SetShaderParameters(ID3D11DeviceContext* pContext, Matrix World, Matrix View, Matrix Proj)
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
	hr = pContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
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
	pContext->Unmap(m_pMatrixBuffer, 0);

	// 정점 쉐이더에서의 상수 버퍼의 위치 설정
	BufferNumber = 0;

	// 정점 쉐이더의 상수 버퍼를 바뀐 값으로 설정
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
	// 상수 버퍼 해제
	if (m_pMatrixBuffer)
	{
		m_pMatrixBuffer->Release();
		m_pMatrixBuffer = 0;
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
