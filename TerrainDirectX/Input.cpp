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
	// 키가 눌리면 상태를 배열에 저장.
	m_Key[key] = true;
	return;
}

void Input::KeyUp(unsigned int key)
{
	// 키를 떼면 상태를 배열에 저장
	m_Key[key] = false;
	return;
}

bool Input::IsKeyDown(unsigned int key)
{
	// 현재 키 상태를 반환
	return m_Key[key];
}