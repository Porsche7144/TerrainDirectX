#pragma once
#ifndef _INPUT_H_
#define _INPUT_H_
#define INPUT_SIZE 256

class Input
{
public:
	bool m_Key[INPUT_SIZE];

public:
	bool Init();
	void KeyDown(unsigned int key);
	void KeyUp(unsigned int key);
	bool IsKeyDown(unsigned int key);

public:
	Input()
	{

	}
	~Input()
	{

	}
};

#endif