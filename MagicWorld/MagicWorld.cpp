// MagicWorld.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../Src/Common/MagicFramework.h"
#pragma comment( linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"" ) //hide cmd windows


int _tmain(int argc, _TCHAR* argv[])
{
    MagicCore::MagicFramework magicFrame;
    magicFrame.Init();
    magicFrame.Run();

	return 0;
}

