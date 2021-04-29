#pragma once
#include "D3dClass.h"

class Camera
{
public:
	Vector3 m_Position;
	Vector3 m_Rotation;
	Matrix m_matView;

public:
	bool Init();
	bool Render();
	bool Release();
	void SetPosition(Vector3 pos);
	void SetRotation(Vector3 rotate);
	Vector3 GetPosition();
	Vector3 GetRotation();
	Matrix GetViewMatrix(Matrix View);


public:
	Camera();
	~Camera();
};

