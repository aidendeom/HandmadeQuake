#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include "Color.h"

static int bufferWidth = 640;
static int bufferHeight = 480;
static int bytesPerPixel = 1;
static BOOL fullscreen = FALSE;
void* backBuffer = NULL;

static int isRunning = 1;


typedef struct dibinfo_s
{
	BITMAPINFOHEADER header;
	RGBQUAD			 acolors[256];
} dibinfo_t;

dibinfo_t bitmapInfo = { 0 };

void DrawRect8(int a_StartX, int a_StartY, int a_Width, int a_Height, int a_PaletteIdx, unsigned char* a_Buffer)
{
	// Don't draw if it's outside the buffer
	if (a_StartX + a_Width < 0 || bufferWidth < a_StartX ||
		a_StartY + a_Height < 0 || bufferHeight < a_StartY)
	{
		return;
	}

	if (a_StartX < 0)
	{
		a_StartX = 0;
	}

	if (a_StartY < 0)
	{
		a_StartY = 0;
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
			a_Buffer[y * bufferWidth + x] = a_PaletteIdx;
		}
	}
}

void DrawRect(int a_StartX, int a_StartY, int a_Width, int a_Height, Color a_Color, uint32_t* a_Buffer)
{
	// Don't draw if it's outside the buffer
	if (a_StartX + a_Width < 0 || bufferWidth < a_StartX ||
		a_StartY + a_Height < 0 || bufferHeight < a_StartY)
	{
		return;
	}

	if (a_StartX < 0)
	{
		a_StartX = 0;
	}

	if (a_StartY < 0)
	{
		a_StartY = 0;
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
			a_Buffer[y * bufferWidth + x] = a_Color.i;
		}
	}
}

LRESULT CALLBACK WindowProc(HWND a_Window, UINT a_Msg, WPARAM a_WParam, LPARAM a_LParam)
{
	LRESULT result = 0;

	switch (a_Msg)
	{
	case WM_DESTROY:
	case WM_KEYUP:
		isRunning = 0;
		break;
	case WM_CREATE:
	default:
		result = DefWindowProc(a_Window, a_Msg, a_WParam, a_LParam);
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
		BITMAPINFOHEADER* h = &bitmapInfo.header;
		h->biSize = sizeof(*h);
		h->biWidth = bufferWidth;
		h->biHeight = -bufferHeight;
		h->biPlanes = 1;
		h->biBitCount = 8 * bytesPerPixel;
		h->biCompression = BI_RGB;
	}

	backBuffer = malloc(bufferWidth * bufferHeight * bytesPerPixel);
	if (!backBuffer)
	{
		MessageBox(NULL, "Failed to malloc backBuffer", "Failure", 0);
		return EXIT_FAILURE;
	}

	if (bytesPerPixel == 1)
	{
		bitmapInfo.acolors[0].rgbRed = 0;
		bitmapInfo.acolors[0].rgbGreen = 0;
		bitmapInfo.acolors[0].rgbBlue = 0;
		for (size_t i = 1; i < 256; ++i)
		{
			bitmapInfo.acolors[i].rgbRed = rand() % 256;
			bitmapInfo.acolors[i].rgbGreen = rand() % 256;
			bitmapInfo.acolors[i].rgbBlue = rand() % 256;
		}
	}
	
	while (isRunning)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (bytesPerPixel == 4)
		{
			uint32_t* ptr = backBuffer;
			for (int h = 0; h < bufferHeight; ++h)
			{
				for (int w = 0; w < bufferWidth; ++w)
				{
					Color c = RAND_COLOR;
					*ptr++ = c.i;
				}
			}
			DrawRect(10, 10, 800, 1000, COLOR_BLUE, backBuffer);
		}

		else if (bytesPerPixel == 1)
		{
			unsigned char* ptr = backBuffer;
			for (int h = 0; h < bufferHeight; ++h)
			{
				for (int w = 0; w < bufferWidth; ++w)
				{
					*ptr++ = rand() % 256;
				}
			}
			DrawRect8(10, 10, 100, 350, 10, backBuffer);
		}

		HDC dc = GetDC(mainWindow);

		StretchDIBits(
			dc,
			0, 0, bufferWidth, bufferHeight,
			0, 0, bufferWidth, bufferHeight,
			backBuffer,
			(BITMAPINFO*)&bitmapInfo,
			DIB_RGB_COLORS,
			SRCCOPY);

		ReleaseDC(mainWindow, dc);
	}

	return EXIT_SUCCESS;
}