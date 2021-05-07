#pragma once
#include "Texture.h"

struct BitmapVertexType
{
	// 구조체 순서도 중요하다..
	// TextureUV를 맨 아래에 놓고 color를 중간에 두면
	// 텍스처가 컬러 영향 받아서 원래의 텍스처 컬러가 뿌려지지 않음.
	// 레이아웃 순서에 영향을 주는 것 같다...
	Vector3 pos;
	Vector2 TextureUV;
	// Vector3 normal;     2D이미지에는 위치 벡터와 텍스쳐 좌표만 필요하다.
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

