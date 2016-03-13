#include <windows.h>
#include <stdio.h>

unsigned long bufferWidth = 640;
unsigned long bufferHeight = 480;
static int isRunning = 1;
void* backBuffer = NULL;

BITMAPINFO bitmapInfo = { 0 };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lPara)
{
	LRESULT result = 0;

	switch (uMsg)
	{
	case WM_DESTROY:
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

	BOOL fullScreen = TRUE;
	if (fullScreen)
	{
		DEVMODE dmScreenSettings = { 0 };
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = bufferWidth;
		dmScreenSettings.dmPelsHeight = bufferHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
		{
			extendedStyle = WS_EX_APPWINDOW;
			style = WS_POPUP;
		}
		else
		{
			fullScreen = FALSE;
		}
	}

	RECT r = { 0 };
	r.right = bufferWidth;
	r.bottom = bufferHeight;
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

	if (fullScreen)
	{
		SetWindowLong(mainWindow, GWL_STYLE, 0);
	}

	ShowWindow(mainWindow, nChowCmd);

	{ /* Define bitmap info */
		BITMAPINFOHEADER* h = &bitmapInfo.bmiHeader;
		h->biSize = sizeof(*h);
		h->biWidth = bufferWidth;
		h->biHeight = bufferHeight;
		h->biPlanes = 1;
		h->biBitCount = 32;
		h->biCompression = BI_RGB;
	}

	backBuffer = malloc(bufferWidth * bufferHeight * 4);
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
		for (size_t h = 0; h < bufferHeight; ++h)
		{
			for (size_t w = 0; w < bufferWidth; ++w)
			{
				char red = rand() % 256;
				char green = rand() % 256;
				char blue = rand() % 256;

				*ptr = ((red << 16) | (green << 8) | blue << 0);
				ptr++;
			}
		}

		HDC dc = GetDC(mainWindow);

		StretchDIBits(
			dc,
			0, 0, bufferWidth, bufferHeight,
			0, 0, bufferWidth, bufferHeight,
			backBuffer,
			&bitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

		ReleaseDC(mainWindow, dc);
	}

	return EXIT_SUCCESS;
}