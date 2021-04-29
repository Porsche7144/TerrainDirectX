#pragma once
#include "D3dClass.h"
#include "Texture.h"

struct VertexType
{
	// 구조체 순서도 중요하다..
	// TextureUV를 맨 아래에 놓고 color를 중간에 두면
	// 텍스처가 컬러 영향 받아서 원래의 텍스처 컬러가 뿌려지지 않음.
	// 레이아웃 순서에 영향을 주는 것 같다...
	Vector3 pos;
	Vector2 TextureUV;
	Vector3 normal;
};

class Model
{
public:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iVertexCount, m_iIndexCount;
	Texture* m_pTextureClass;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename);
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();
	bool ReleaseBuffer();
	bool ReleaseTexture();
	bool RenderBuffer(ID3D11DeviceContext* pContext);
	bool InitBuffers(ID3D11Device* pDevice);
	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const TCHAR* Filename);
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

public:
	Model();
	~Model();
};

