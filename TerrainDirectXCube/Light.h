#pragma once
#include "Texture.h"

class Light
{
public:
	Vector4 m_DiffuseColor;
	Vector3 m_Direction;

public:
	void SetDiffuseColor(Vector4 color);
	void SetDirection(Vector3 Dir);
	Vector4 GetDiffuseColor();
	Vector3 GetDirection();

public:
	Light();
	~Light();
};

