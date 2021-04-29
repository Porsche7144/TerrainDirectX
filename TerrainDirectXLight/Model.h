#pragma once
#include "D3dClass.h"
#include "Texture.h"

struct VertexType
{
	// ����ü ������ �߿��ϴ�..
	// TextureUV�� �� �Ʒ��� ���� color�� �߰��� �θ�
	// �ؽ�ó�� �÷� ���� �޾Ƽ� ������ �ؽ�ó �÷��� �ѷ����� ����.
	// ���̾ƿ� ������ ������ �ִ� �� ����...
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

