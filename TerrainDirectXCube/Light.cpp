#include "Light.h"

void Light::SetAmbientColor(Vector4 AmbientColor)
{
	m_AmbientColor = AmbientColor;
	return;
}

void Light::SetSpecularColor(Vector4 color)
{
	m_SpecularColor = color;
	return;
}

void Light::SetSpecularPower(float fSpecPower)
{
	m_fSpecularPower = fSpecPower;
	return;
}

void Light::SetDiffuseColor(Vector4 color)
{
	m_DiffuseColor = color;
	return;
}

void Light::SetDirection(Vector3 Dir)
{
	m_Direction = Dir;
	return;
}

Vector4 Light::GetDiffuseColor()
{
	return m_DiffuseColor;
}

Vector4 Light::GetAmbientColor()
{
	return m_AmbientColor;
}

Vector4 Light::GetSpecularColor()
{
	return m_SpecularColor;
}

float Light::GetSpecularPower()
{
	return m_fSpecularPower;
}

Vector3 Light::GetDirection()
{
	return m_Direction;
}

Light::Light()
{
}

Light::~Light()
{
}
