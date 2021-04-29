#include "systemclass.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;


	// �ý��� ��ü ����.
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// �ý��� ��ü�� �ʱ�ȭ�ϰ� Runȣ��.
	result = System->Init();
	if (result)
	{
		System->Run();
	}

	// �ý��� ��ü�� �����ϰ� �޸� ��ȯ.
	System->Release();
	delete System;
	System = 0;

	return 0;
}