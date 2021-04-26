#include "Input.h"

bool Input::Init()
{
	for (int i = 0; i < INPUT_SIZE; i++)
	{
		m_Key[i] = false;
	}

	return true;
}

void Input::KeyDown(unsigned int key)
{
	// Ű�� ������ ���¸� �迭�� ����.
	m_Key[key] = true;
	return;
}

void Input::KeyUp(unsigned int key)
{
	// Ű�� ���� ���¸� �迭�� ����
	m_Key[key] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	// ���� Ű ���¸� ��ȯ
	return m_Key[key];
}