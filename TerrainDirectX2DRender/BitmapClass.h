#pragma once
#include "Texture.h"

struct BitmapVertexType
{
	// ����ü ������ �߿��ϴ�..
	// TextureUV�� �� �Ʒ��� ���� color�� �߰��� �θ�
	// �ؽ�ó�� �÷� ���� �޾Ƽ� ������ �ؽ�ó �÷��� �ѷ����� ����.
	// ���̾ƿ� ������ ������ �ִ� �� ����...
	Vector3 pos;
	Vector2 TextureUV;
	// Vector3 normal;     2D�̹������� ��ġ ���Ϳ� �ؽ��� ��ǥ�� �ʿ��ϴ�.
};

class BitmapClass
{
public:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iVertexCount, m_iIndexCount;
	Texture* m_pTextureClass;

	int m_ScreenWidth, m_ScreenHeight;
	int m_BitmapWidth, m_BitmapHeight;
	int m_PrevPosX, m_PrevPosY;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename,
			  int ScreenWidht, int ScreenHeight, int bitmapWidth, int bitmapHeight);
	bool Render(ID3D11DeviceContext* pContext, int PosX, int PosY);
	bool Release();
	bool ReleaseBuffer();
	bool ReleaseTexture();
	bool RenderBuffer(ID3D11DeviceContext* pContext);
	bool InitBuffers(ID3D11Device* pDevice);
	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename);
	bool UpdateBuffer(ID3D11DeviceContext* pContext, int PosX, int PosY);
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();
	bool LoadModelText(const TCHAR* texName);
	bool ReleaseModelText();

public:
	BitmapClass();
	~BitmapClass();
};

