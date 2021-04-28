#pragma once
#include "D3dClass.h"
#include "Texture.h"

struct VertexType
{
	Vector3 pos;
	Vector4 color;
	Vector2 TextureUV;
};

class Model
{
public:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iVertexCount, m_iIndexCount;
	Texture* m_pTextureClass;

public:
	bool Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string Filename);
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();
	bool ReleaseBuffer();
	bool ReleaseTexture();
	bool RenderBuffer(ID3D11DeviceContext* pContext);
	bool InitBuffers(ID3D11Device* pDevice);
	bool LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string Filename);
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

public:
	Model();
	~Model();
};

