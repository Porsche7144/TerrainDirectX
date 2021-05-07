#include "FontClass.h"

ID3D11ShaderResourceView* FontClass::GetFontTexture()
{
	return m_pTexture->GetTexture();
}

bool FontClass::LoadFontData(const char* fontFileName)
{
	ifstream fin;
	int i;
	char temp;

	// FontType 배열 생성.  텍스쳐 글자수, txt파일 인덱스 개수만큼
	m_pFontType = new FontType[95];
	if (!m_pFontType)
	{
		return false;
	}

	fin.open(fontFileName);
	if (fin.fail())
	{
		return false;
	}

	// 텍스트에 사용되는 95개 문자를 읽어온다
	for (int i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_pFontType[i].left;
		fin >> m_pFontType[i].right;
		fin >> m_pFontType[i].size;

	}

	fin.close();


	return true;
}

bool FontClass::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const WCHAR* textureFileName)
{
	bool result;

	// 텍스쳐 객체 생성
	m_pTexture = new Texture;
	if (!m_pTexture)
	{
		return false;
	}

	// 텍스쳐 Init
	result = m_pTexture->Init(pDevice, pContext, textureFileName);
	if (!m_pTexture)
	{
		return false;
	}


	return true;
}

bool FontClass::BuildVertexArray(void* vertices, const char* sentence, float DrawX, float DrawY)
{
	// BuildVertexArray 함수는 TextClass에 의해 호출되어 인자로 받은 문장으로 정점 버퍼를 만들어낸다.
	// TextClass는 이렇게 만들어진 자신만의 정점 버퍼를 가지게 되기 때문에 손쉽게 글자를 그릴 수 있다.
	// 첫번째 인자인 vertices는 정점 배열을 가리키는 포인터로서 TextClass에게 이 함수에서 내용을 채운 배열을 
	// 전달하는 데 사용된다. 그 다음 인자인 sentence는 정점 배열을 만드는 데 필요한 문장(문자열)이다.
	// drawX와 drawY는 문장이 그려질 화면상의 좌표.
	FontVertexType* vertexPtr;
	int numLetters, index, letter;

	// 받아온 정점데이터를 정점 유형 구조로 강제적용
	vertexPtr = (FontVertexType*)vertices;

	// 문장의 글자 수를 구함
	numLetters = (int)strlen(sentence);

	// 정점 배열의 인덱스 수 초기화
	index = 0;

	for (int i = 0; i < numLetters; i++)
	{

	}


	return true;
}

bool FontClass::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext ,const char* fontFilename, const WCHAR* textrueFileName)
{
	bool result;

	// 폰트파일 로드
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// 텍스쳐 파일 로드
	result = LoadTexture(pDevice, pContext, textrueFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

bool FontClass::Release()
{

	if (m_pFontType)
	{
		delete[] m_pFontType;
		m_pFontType = 0;
	}

	if (m_pTexture)
	{
		m_pTexture->Release();
		delete m_pTexture;
		m_pTexture = 0;
	}

	return true;
}

FontClass::FontClass()
{
	m_pFontType = nullptr;
	m_pTexture = nullptr;
}

FontClass::~FontClass()
{
}
