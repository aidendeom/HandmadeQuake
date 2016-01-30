#include <windows.h>
#include <stdio.h>

static BOOL isRunning = TRUE;

void Sys_Shutdown()
{
	isRunning = FALSE;
}


static BOOL timeInitialized = FALSE;
static __int64 ticksPerSecond = 0;
static __int64 ticksAtProgramStart = 0;

/*
* Get the number of seconds since program start
*/
float Sys_GetFloatTime()
{
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

	__int64 tickDiff = t.QuadPart - ticksAtProgramStart;
	double secDiff = (double)tickDiff / ticksPerSecond;

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
		"Lesson 2.4",
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

	MSG msg;
	while (isRunning)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float seconds = Sys_GetFloatTime();

		char buff[64];
		sprintf_s(buff, 64, "Time: %3.7f\n", seconds);
		OutputDebugString(buff);

		Sleep(10);
	}

	return 0;
}