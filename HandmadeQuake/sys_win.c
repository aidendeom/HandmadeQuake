#include "quakedef.h"
#include "winquake.h"

enum { TARGET_FRAMERATE = 60 };
static float TARGET_TIMESTEP = 1.0f / TARGET_FRAMERATE;

static BOOL isRunning = TRUE;

void Sys_Shutdown(void)
{
	isRunning = FALSE;
}

float Sys_FloatTime()
{
	static BOOL timeInitialized = FALSE;
	static __int64 ticksPerSecond = 0;
	static __int64 ticksAtProgramStart = 0;

	double secDiff = 0.0;
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	if (!timeInitialized)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		ticksPerSecond = frequency.QuadPart;

		ticksAtProgramStart = t.QuadPart;
		timeInitialized = TRUE;
	}
	else
	{
		__int64 tickDiff = t.QuadPart - ticksAtProgramStart;
		secDiff = (double)tickDiff / ticksPerSecond;
	}

	return (float)secDiff;
}

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = 0;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			isRunning = FALSE;
		break;
	case WM_CLOSE:
		Sys_Shutdown();
		PostQuitMessage(0);
		break;
	default:
		res = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return res;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// Make compiler happy
	lpCmdLine;
	nShowCmd;
	hPrevInstance;

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = MainWinProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = "QuakeWindow";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "RegisterClass failed", "Initialization Failure", 0);
		return EXIT_FAILURE;
	}

	HWND mainWindow;
	DWORD windowStyle = WS_OVERLAPPEDWINDOW;

	RECT r = { 0 };
	r.right = 800;
	r.bottom = 600;
	AdjustWindowRect(&r, windowStyle, FALSE);

	mainWindow = CreateWindowEx(
		0,
		"QuakeWindow",
		"Module 2",
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!mainWindow)
	{
		MessageBox(NULL, "CreateWindow failed", "Initialization Failure", 0);
		return EXIT_FAILURE;
	}

	ShowWindow(mainWindow, SW_SHOWDEFAULT);

	HDC ctx = GetDC(mainWindow);
	PatBlt(ctx, 0, 0, 800, 600, BLACKNESS);
	ReleaseDC(mainWindow, ctx);

	Host_Init();

	float oldtime = Sys_FloatTime();

	MSG msg;
	while (isRunning)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float newtime = Sys_FloatTime();
		Host_Frame(newtime - oldtime);
		oldtime = newtime;
	}

	Host_Shutdown();

	return 0;
}