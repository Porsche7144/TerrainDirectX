#include "Camera.h"

void Camera::SetPosition(Vector3 pos)
{
	m_Position.x = pos.x;
	m_Position.y = pos.y;
	m_Position.z = pos.z;
}

void Camera::SetRotation(Vector3 rotate)
{
	m_Rotation.x = rotate.x;
	m_Rotation.y = rotate.y;
	m_Rotation.z = rotate.z;
}

Vector3 Camera::GetPosition()
{
	return m_Position;
}

Vector3 Camera::GetRotation()
{
	return m_Rotation;
}

Matrix Camera::GetViewMatrix(Matrix View)
{
	View = m_matView;
	return View;
}

bool Camera::Init()
{
	return true;
}

bool Camera::Render()
{
	Vector3 up, pos, lookAt;
	float yaw, pitch, roll;
	Matrix matRotation;

	// ������ ����
	up = Vector3(0.0f, 1.0f, 0.0f);

	// ī�޶� ���� ����
	pos = Vector3(m_Position.x, m_Position.y, m_Position.z);

	// Look���� ����Ʈ ����
	lookAt = Vector3(0.0f, 0.0f, 1.0f);

	// ���Ϸ� ��
	// ī�޶��� ȸ���� ī�޶� ��ȯ�� ���
	// Yaw(y), Pitch(x), Roll(z) ȸ���� ���Ȱ����� Convert
	pitch = ConvertToRadians(m_Rotation.x);
	yaw = ConvertToRadians(m_Rotation.y);
	roll = ConvertToRadians(m_Rotation.z);

	// Yaw(y), Pitch(x), Roll(z) ��� ����
	matRotation = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);

	// �������͸� ī�޶� ȸ����ķ� ��ȯ
	// ���ο� ī�޶� ��ġ = ���� ��ġ * ���յ� ȸ�����
	lookAt = Vector3::Transform(lookAt, matRotation);
	up = Vector3::Transform(up, matRotation);

	// ȸ���� ī�޶� ��ġ�� �� ��ġ�� ��ȯ
	lookAt = pos + lookAt;

	// �� ��Ʈ���� ����
	m_matView = m_matView.CreateLookAt(pos, lookAt, up);


	return true;
}

bool Camera::Release()
{
	return true;
}

Camera::Camera()
{
	// ī�޶� ��ġ�� ȸ�����¸� �������� �ʱ�ȭ
	m_Position = { 0.0f, 0.0f, 0.0f };
	m_Rotation = { 0.0f, 0.0f, 0.0f };
}

Camera::~Camera()
{

}
