#include <windows.h>
#include <stdio.h>

BOOL isRunning = TRUE;

void Sys_Shutdown()
{
	isRunning = FALSE;
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

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER t1;
	LARGE_INTEGER t2;
	QueryPerformanceCounter(&t1);

	MSG msg;
	while (isRunning)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		QueryPerformanceCounter(&t2);
		__int64 tickDiff = t2.QuadPart - t1.QuadPart;
		double secDiff = (double)tickDiff / frequency.QuadPart;
		QueryPerformanceCounter(&t1);
	}

	return 0;
}