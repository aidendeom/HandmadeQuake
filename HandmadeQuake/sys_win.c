#include "winquake.h"
#include "quakedef.h"


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	COM_ParseCmdLine(lpCmdLine);

	int test = COM_CheckParm("-setalpha");
	int value = -1;
	if (test)
	{
		value = Q_atoi(com_argv[test + 1]);
	}
	return 0;
}