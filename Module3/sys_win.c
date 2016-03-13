#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "Color.h"

static int bufferWidth = 640;
static int bufferHeight = 480;
static int bytesPerPixel = 4;
static int isRunning = 1;
static BOOL fullscreen = FALSE;

Color* m_backBuffer = NULL;
BITMAPINFO bitmapInfo = { 0 };

void DrawRect(int a_StartX, int a_StartY, int a_Width, int a_Height, Color a_Color, Color* a_Buffer)
{
	// Don't draw if it's outside the buffer
	if (a_StartX < 0 || bufferWidth < a_StartX ||
		a_StartY < 0 || bufferHeight < a_StartY)
	{
		return;
	}

	// Clamp width and height
	if (a_StartX + a_Width > bufferWidth)
	{
		a_Width = bufferWidth - a_StartX;
	}
	if (a_StartY + a_Height > bufferHeight)
	{
		a_Height = bufferHeight - a_StartY;
	}

	for (int y = a_StartY; y < a_StartY + a_Height; ++y)
	{
		for (int x = a_StartX; x < a_StartX + a_Width; ++x)
		{
			a_Buffer[y * bufferWidth + x] = a_Color;
		}
	}
}

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

	if (fullscreen)
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
			fullscreen = FALSE;
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

	if (fullscreen)
	{
		SetWindowLong(mainWindow, GWL_STYLE, 0);
	}

	ShowWindow(mainWindow, nChowCmd);

	{ /* Define bitmap info */
		BITMAPINFOHEADER* h = &bitmapInfo.bmiHeader;
		h->biSize = sizeof(*h);
		h->biWidth = bufferWidth;
		h->biHeight = -bufferHeight;
		h->biPlanes = 1;
		h->biBitCount = 32;
		h->biCompression = BI_RGB;
	}

	m_backBuffer = (Color*)malloc(bufferWidth * bufferHeight * bytesPerPixel);
	Color* const backBuffer = m_backBuffer;
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

		Color* ptr = backBuffer;
		for (int h = 0; h < bufferHeight; ++h)
		{
			for (int w = 0; w < bufferWidth; ++w)
			{
				Color pc = { 0 };
				pc.r = rand() % 256;
				pc.g = rand() % 256;
				pc.b = rand() % 256;

				*ptr++ = pc;
			}
		}

		DrawRect(100, 100, 4000000, 300, COLOR_GREEN, backBuffer);

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