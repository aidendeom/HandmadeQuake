#include <windows.h>

BOOL isRunning = TRUE;

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
		isRunning = FALSE;
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
	wc.lpszClassName = "Module 2";

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, "RegisterClass failed", "Module 2", 0);
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
		"Module 2",
		"Lesson 2.1",
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(mainWindow, SW_SHOWDEFAULT);

	HDC ctx = GetDC(mainWindow);
	PatBlt(ctx, 0, 0, 800, 600, BLACKNESS);
	ReleaseDC(mainWindow, ctx);

	MSG msg;
	while (isRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}