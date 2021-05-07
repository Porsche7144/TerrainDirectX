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

	// FontType �迭 ����.  �ؽ��� ���ڼ�, txt���� �ε��� ������ŭ
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

	// �ؽ�Ʈ�� ���Ǵ� 95�� ���ڸ� �о�´�
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

	// �ؽ��� ��ü ����
	m_pTexture = new Texture;
	if (!m_pTexture)
	{
		return false;
	}

	// �ؽ��� Init
	result = m_pTexture->Init(pDevice, pContext, textureFileName);
	if (!m_pTexture)
	{
		return false;
	}


	return true;
}

bool FontClass::BuildVertexArray(void* vertices, const char* sentence, float DrawX, float DrawY)
{
	// BuildVertexArray �Լ��� TextClass�� ���� ȣ��Ǿ� ���ڷ� ���� �������� ���� ���۸� ������.
	// TextClass�� �̷��� ������� �ڽŸ��� ���� ���۸� ������ �Ǳ� ������ �ս��� ���ڸ� �׸� �� �ִ�.
	// ù��° ������ vertices�� ���� �迭�� ����Ű�� �����ͷμ� TextClass���� �� �Լ����� ������ ä�� �迭�� 
	// �����ϴ� �� ���ȴ�. �� ���� ������ sentence�� ���� �迭�� ����� �� �ʿ��� ����(���ڿ�)�̴�.
	// drawX�� drawY�� ������ �׷��� ȭ����� ��ǥ.
	FontVertexType* vertexPtr;
	int numLetters, index, letter;

	// �޾ƿ� ���������͸� ���� ���� ������ ��������
	vertexPtr = (FontVertexType*)vertices;

	// ������ ���� ���� ����
	numLetters = (int)strlen(sentence);

	// ���� �迭�� �ε��� �� �ʱ�ȭ
	index = 0;

	for (int i = 0; i < numLetters; i++)
	{

	}


	return true;
}

bool FontClass::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext ,const char* fontFilename, const WCHAR* textrueFileName)
{
	bool result;

	// ��Ʈ���� �ε�
	result = LoadFontData(fontFilename);
	if (!result)
	{
		return false;
	}

	// �ؽ��� ���� �ε�
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
