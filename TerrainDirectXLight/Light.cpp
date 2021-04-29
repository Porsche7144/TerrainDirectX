#include "Light.h"

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
