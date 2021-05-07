#pragma once
#include "Texture.h"

class Light
{
public:
	Vector4 m_AmbientColor;
	Vector4 m_DiffuseColor;
	Vector4 m_SpecularColor;
	Vector3 m_Direction;
	float m_fSpecularPower;

public:
	void SetAmbientColor(Vector4 AmbientColor);
	void SetDiffuseColor(Vector4 color);
	void SetDirection(Vector3 Dir);
	void SetSpecularColor(Vector4 color);
	void SetSpecularPower(float fSpecPower);
	Vector4 GetAmbientColor();
	Vector4 GetDiffuseColor();
	Vector3 GetDirection();
	Vector4 GetSpecularColor();
	float GetSpecularPower();

public:
	Light();
	~Light();
};

