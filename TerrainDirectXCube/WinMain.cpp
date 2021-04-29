#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;


	// 시스템 객체 생성.
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// 시스템 객체를 초기화하고 Run호출.
	result = System->Init();
	if (result)
	{
		System->Run();
	}

	// 시스템 객체를 종료하고 메모리 반환.
	System->Release();
	delete System;
	System = 0;

	return 0;
}