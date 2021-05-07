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

	// 업벡터 설정
	up = Vector3(0.0f, 1.0f, 0.0f);

	// 카메라 월드 세팅
	pos = Vector3(m_Position.x, m_Position.y, m_Position.z);

	// Look벡터 디폴트 설정
	lookAt = Vector3(0.0f, 0.0f, 1.0f);

	// 오일러 각
	// 카메라의 회전과 카메라 전환에 사용
	// Yaw(y), Pitch(x), Roll(z) 회전을 라디안값으로 Convert
	pitch = ConvertToRadians(m_Rotation.x);
	yaw = ConvertToRadians(m_Rotation.y);
	roll = ConvertToRadians(m_Rotation.z);

	// Yaw(y), Pitch(x), Roll(z) 행렬 생성
	matRotation = Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);

	// 기저백터를 카메라 회전행렬로 변환
	// 새로운 카메라 위치 = 현재 위치 * 결합된 회전행렬
	lookAt = Vector3::Transform(lookAt, matRotation);
	up = Vector3::Transform(up, matRotation);

	// 회전된 카메라 위치를 뷰 위치로 변환
	lookAt = pos + lookAt;

	// 뷰 매트릭스 생성
	m_matView = m_matView.CreateLookAt(pos, lookAt, up);


	return true;
}

bool Camera::Release()
{
	return true;
}

Camera::Camera()
{
	// 카메라 위치와 회전상태를 원점으로 초기화
	m_Position = { 0.0f, 0.0f, 0.0f };
	m_Rotation = { 0.0f, 0.0f, 0.0f };
}

Camera::~Camera()
{

}
