#include <windows.h>

enum /* Constants */
{
	WINDOW_WIDTH = 640,
	WINDOW_HEIGHT = 480,
};

static int isRunning = 1;
void* backBuffer = NULL;

BITMAPINFO bitmapInfo = { 0 };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lPara)
{
	LRESULT result = 0;

	switch (uMsg)
	{
	case WM_KEYUP:
		isRunning = 0;
		break;
	case WM_CREATE:
	default:
		result = DefWindowProc(hWnd, uMsg, wParam, lPara);
	}

	return result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nChowCmd)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "QuakeWindowClass";

	RegisterClassEx(&wc);

	DWORD extendedStyle = 0;
	DWORD style = WS_OVERLAPPEDWINDOW;

	RECT r = { 0 };
	r.right = WINDOW_WIDTH;
	r.bottom = WINDOW_HEIGHT;
	AdjustWindowRectEx(&r, style, 0, extendedStyle);

	HWND mainWindow = CreateWindowEx(
		extendedStyle,
		"QuakeWindowClass",
		"Module 3",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		NULL,
		NULL,
		hInstance,
		0);

	ShowWindow(mainWindow, nChowCmd);

	{ /* Define bitmap info */
		BITMAPINFOHEADER* h = &bitmapInfo.bmiHeader;
		h->biSize = sizeof(*h);
		h->biWidth = WINDOW_WIDTH;
		h->biHeight = WINDOW_HEIGHT;
		h->biPlanes = 1;
		h->biBitCount = 32;
		h->biCompression = BI_RGB;
	}

	backBuffer = malloc(WINDOW_WIDTH * WINDOW_HEIGHT * 4);
	if (!backBuffer)
	{
		MessageBox(NULL, "Failed to malloc backBuffer", "Failure", 0);
		return EXIT_FAILURE;
	}

	
	while (isRunning)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		int* ptr = (int*)backBuffer;
		for (size_t h = 0; h < WINDOW_HEIGHT; ++h)
		{
			for (size_t w = 0; w < WINDOW_WIDTH; ++w)
			{
				char red = 0xFF;
				char green = 0;
				char blue = 0;

				*ptr = ((red << 16) | (green << 8) | blue);
				ptr++;
			}
		}

		HDC dc = GetDC(mainWindow);

		StretchDIBits(
			dc,
			0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			0, 0, WINDOW_WIDTH, WINDOW_HEIGHT,
			backBuffer,
			&bitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

		ReleaseDC(mainWindow, dc);
	}

	return EXIT_SUCCESS;
}