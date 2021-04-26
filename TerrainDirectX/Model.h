#pragma once
#include "Graphics.h"

struct VertexType
{
	Vector3 pos;
	Vector4 color;
};

class Model
{
public:
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	int m_iVertexCount, m_iIndexCount;

public:
	bool Init();
	bool Render(ID3D11DeviceContext* pContext);
	bool Release();
	bool ReleaseBuffer();
	bool RenderBuffer(ID3D11DeviceContext* pContext);
	bool InitBuffers(ID3D11Device* pDevice);
	int GetIndexCount();

public:
	Model();
	~Model();
};

