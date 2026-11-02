#include "screensaver.h"

GDI_PAYLOAD gdiPayloads[20] = { 0 };

HWND g_hWnd = NULL;

static INT w = 1;
static INT h = 1;

VOID InitializeGdiPayloads()
{
	gdiPayloads[0] = (GDI_PAYLOAD) { Payload1, LayeredPayload1 };
	gdiPayloads[1] = (GDI_PAYLOAD) { Payload2, LayeredPayload2 };
	gdiPayloads[2] = (GDI_PAYLOAD) { Payload3, LayeredPayload3 };
	gdiPayloads[3] = (GDI_PAYLOAD) { Payload4, LayeredPayload4 };
	gdiPayloads[4] = (GDI_PAYLOAD) { Payload5, LayeredPayload5 };
	gdiPayloads[5] = (GDI_PAYLOAD) { Payload6, LayeredPayload6 };
	gdiPayloads[6] = (GDI_PAYLOAD) { Payload7, LayeredPayload7 };
	gdiPayloads[7] = (GDI_PAYLOAD) { Payload8, LayeredPayload8 };
	gdiPayloads[8] = (GDI_PAYLOAD) { Payload9, LayeredPayload9 };
	gdiPayloads[9] = (GDI_PAYLOAD) { Payload10, LayeredPayload10 };
	gdiPayloads[10] = (GDI_PAYLOAD) { Payload11, LayeredPayload11 };
	gdiPayloads[11] = (GDI_PAYLOAD) { Payload12, LayeredPayload12 };
	gdiPayloads[12] = (GDI_PAYLOAD) { Payload13, LayeredPayload13 };
	gdiPayloads[13] = (GDI_PAYLOAD) { Payload14, LayeredPayload14 };
	gdiPayloads[14] = (GDI_PAYLOAD) { Payload15, LayeredPayload15 };
	gdiPayloads[15] = (GDI_PAYLOAD) { Payload16, LayeredPayload16 };
	gdiPayloads[16] = (GDI_PAYLOAD){ Payload17, LayeredPayload17 };
	gdiPayloads[17] = (GDI_PAYLOAD){ Payload18, LayeredPayload18 };
	gdiPayloads[18] = (GDI_PAYLOAD){ Payload19, LayeredPayload19 };
	gdiPayloads[19] = (GDI_PAYLOAD){ Payload20, LayeredPayload20 };

	if (!g_bIs102004OrNewer) return;

	for (INT i = 0; i < _countof(gdiPayloads); i++)
		gdiPayloads[i].fnNormal = NULL;
}

DWORD WINAPI GdiThread(LPVOID lv)
{
	InitializeGdiPayloads();
	for (;;)
	{
		BOOL bAllNulls = TRUE;
		for (INT i = 0; i < _countof(gdiPayloads); i++)
		{
			if ((gdiPayloads[i].fnNormal != NULL) || (gdiPayloads[i].fnLayered != NULL)) bAllNulls = FALSE;
		}
		if (bAllNulls) return 1;

		INT nIndex;
		do
		{
			nIndex = Xorshift32() % _countof(gdiPayloads);
		} while (gdiPayloads[nIndex].fnLayered == NULL);

		InvalidateRect(NULL, NULL, TRUE);
		ExecutePayload(nIndex, Xorshift32() % 10000 + 10000);

		gdiPayloads[nIndex].fnNormal = NULL;
		gdiPayloads[nIndex].fnLayered = NULL;
	} lv;
}

VOID ExecutePayload(INT nIndex, DWORD dwDuration)
{
	HANDLE hNormalPayload = NULL;
	HANDLE hLayeredPayload = QuickCreateThread(gdiPayloads[nIndex].fnLayered, NULL);

	if (gdiPayloads[nIndex].fnNormal != NULL)
		hNormalPayload = QuickCreateThread(gdiPayloads[nIndex].fnNormal, NULL);

	Sleep(dwDuration);

	if (hNormalPayload)
	{
		TerminateThread(hNormalPayload, 0);
		CloseHandle(hNormalPayload);
	}

	TerminateThread(hLayeredPayload, 0);
	CloseHandle(hLayeredPayload);
}

DWORD WINAPI Payload1(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		INT rx = Xorshift32() % w;

		BitBlt(hdc, rx, Xorshift32() % 10, 100, h, hdc, rx, Xorshift32() % 10, Xorshift32() % 2 == 0 ? SRCPAINT : SRCAND);

		if (!(Xorshift32() % 16))
			BitBlt(hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32() % w, Xorshift32() % h, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());

		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 0xFF)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload1(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		BYTE* pRgbValues = (BYTE*)ppvBits;

		SineWave(mhdc, (FLOAT)nTicks * 20.0f, 0.02f, 12.0f, 1, FALSE);
		SineWave(mhdc, (FLOAT)nTicks * 20.0f, 0.02f, 12.0f, 1, TRUE);

		SawtoothWave(mhdc, (FLOAT)nTicks * 20.0f, 0.02f, 5.0f, 1, TRUE);
		SawtoothWave(mhdc, (FLOAT)nTicks * 10.0f, 0.02f, 3.0f, 1, TRUE);

		RGB rgb = HSLtoRGB((HSL) { (FLOAT)nTicks, 1.0f, 0.5f });

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				BYTE bFormula = (BYTE)((((x + nTicks * 20) ^ (y + nTicks * 8)) + nTicks * 20) / 8);

				pRgbValues[nIndex] = (BYTE)(b & (rgb.b & bFormula));
				pRgbValues[nIndex + 1] = (BYTE)(g & (rgb.g & bFormula));
				pRgbValues[nIndex + 2] = (BYTE)(r & (rgb.r & bFormula));
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload2(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(mhdc, hbit);

		HFONT hFont = GetWingdingsFont(96);
		SelectObject(hdc, hFont);

		INT rx = Xorshift32() % w;
		INT ry = Xorshift32() % h;
		INT rw = Xorshift32() % w;
		INT rh = Xorshift32() % h;

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		BitBlt(mhdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, rw, rh, mhdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);
		AlphaBlend(hdc, 0, 0, w, h, mhdc, 0, 0, w, h, (BLENDFUNCTION) { 0, 0, 150, 0 });

		DrawIcon(hdc, Xorshift32() % w, Xorshift32() % h, LoadIconW(NULL, IDI_ERROR));

		SetTextColor(hdc, 0x0000FF);
		SetBkMode(hdc, TRANSPARENT);

		TextOutW(hdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

		if (!(Xorshift32() % 8))
			BitBlt(hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32() % w, Xorshift32() % h, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());

		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteObject(hbit);
		DeleteObject(hFont);

		if (!(Xorshift32() % 0x4F)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload2(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		INT sw = ((INT)(FLOAT)(w * (1.0f + ((cosf((FLOAT)nTicks / 5.0f) + 1.0f) / 3.0f))));
		INT sh = ((INT)(FLOAT)(h * (1.0f + ((cosf((FLOAT)nTicks / 5.0f) + 1.0f) / 3.0f))));

		StretchBlt(mhdc, -((sw - w) / 2), -((sh - h) / 2), sw, sh, hdc, 0, 0, w, h, NOTSRCCOPY);

		SineWave(mhdc, (FLOAT)nTicks * 4.0f, 0.5f, 7.4f, 1, TRUE);
		SawtoothWave(mhdc, (FLOAT)nTicks * 20.0f, 0.01f, 20.0f, 1, TRUE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				HSL hsl = RGBtoHSL((RGB) { r, g, b });

				FLOAT fGradient = ((x + y) / 16.0f) + nTicks * 15.0f;
				FLOAT fXorFractal = ScaleToHueRange(((x + (INT)(y * tanf((FLOAT)nTicks * 0.1f) + 1.0f)) ^ (y + (INT)(x * sinf(nTicks * 0.1f)))) + (nTicks * 10.0f));

				RGB rgb = HSLtoRGB((HSL) { (FLOAT)((INT)fGradient ^ (INT)fXorFractal), hsl.s * 0.5f + 0.5f, hsl.l });

				pRgbValues[nIndex] = (BYTE)(rgb.b);
				pRgbValues[nIndex + 1] = (BYTE)(rgb.g);
				pRgbValues[nIndex + 2] = (BYTE)(rgb.r);
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload3(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		INT nSize = 200;
		INT rx = Xorshift32() % (w - nSize);
		INT ry = Xorshift32() % (h - nSize);

		BitBlt(hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, nSize, nSize, hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);

		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 0x7FF)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload3(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;
	FLOAT fRadius = h / 1.5f;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		INT nOffsetX = (INT)(cosf((FLOAT)nTicks / 5.0f) * fRadius);
		INT nOffsetY = (INT)(sinf((FLOAT)nTicks / 5.0f) * fRadius);

		for (INT i = 0; i < 2; i++)
		{
			for (INT j = -1; j < 2; j++)
			{
				POINT points[3] = { 0 };

				INT w1 = (INT)(w * ((cosf((FLOAT)nTicks / 5.0f) + 1.0f) / 2.0f + 1.0f));
				INT x = w1 * i + (nOffsetX * i);
				INT y = h * j + (nOffsetY * i);

				points[0].x = x;
				points[0].y = y;
				points[1].x = x + w1 + nOffsetX;
				points[1].y = y + nOffsetY;
				points[2].x = x;
				points[2].y = h + y;

				PlgBlt(mhdc, points, hdc, 0, 0, w, h, NULL, 0, 0);
			}
		}

		FLOAT cx = w / 2.0f;
		FLOAT cy = h / 2.0f;
		FLOAT fRadius1 = sqrtf(cx * cx + cy * cy);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				FLOAT dx = x - cx;
				FLOAT dy = y - cy;
				FLOAT fDist = sqrtf(dx * dx + dy * dy);

				RGB rgb = HSLtoRGB((HSL) { ((fDist / fRadius1) * 360.0f) + (FLOAT)(nTicks * 10.0f), fabsf(sinf((FLOAT)nTicks / 4.0f)), 0.5f });

				pRgbValues[nIndex] = (BYTE)(b | rgb.b);
				pRgbValues[nIndex + 1] = (BYTE)(g | rgb.g);
				pRgbValues[nIndex + 2] = (BYTE)(r | rgb.r);
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload4(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nSize = 120;
	INT nSpeed = 20;
	INT cx = 0;
	INT cy = 0;
	BOOL incx = TRUE;
	BOOL incy = TRUE;
	FLOAT hue = 0.0f;

	for (;;)
	{
		for (DWORD i = 0; i < Xorshift32() % 20; i++)
		{
			HDC hdc = GetDC(NULL);

			RGB rgb = HSLtoRGB((HSL) { hue, 1.0f, 0.5f });

			HPEN hPen = CreatePen(PS_SOLID, 3, 0);
			SelectObject(hdc, hPen);

			HBRUSH hBrush = CreateSolidBrush(RGB(rgb.r, rgb.g, rgb.b));
			SelectObject(hdc, hBrush);

			Ellipse(hdc, cx, cy, cx + nSize, cy + nSize);

			INT rw = Xorshift32() % w;
			INT rh = Xorshift32() % h;
			INT rx = Xorshift32() % w + rw;
			INT ry = Xorshift32() % h + rh;

			BitBlt(hdc,
				rx + Xorshift32() % 5 - rw, ry + Xorshift32() % 5 - rh,
				rw, rh, hdc,
				rx + Xorshift32() % 5 - rw, ry + Xorshift32() % 5 - rh,
				Xorshift32());

			Sleep(40);

			cx = incx ? cx + nSpeed : cx - nSpeed;
			cy = incy ? cy + nSpeed : cy - nSpeed;
			if (cx + 100 >= w) incx = FALSE;
			if (cy + 100 >= h) incy = FALSE;
			if (cx <= 0) incx = TRUE;
			if (cy <= 0) incy = TRUE;

			hue = fmodf(hue + 2.0f, 360.0f);

			ReleaseDC(NULL, hdc);
			DeleteObject(hBrush);
			DeleteObject(hPen);
		}
		InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload4(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, NOTSRCCOPY);
		SineWave(mhdc, (FLOAT)nTicks * 25.0f, 0.02f, 35.0f, 1, TRUE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		BOOL bShift1 = Xorshift32() % 2;
		BOOL bShift2 = Xorshift32() % 2;
		BOOL bShift3 = Xorshift32() % 2;

		INT nShift1 = Xorshift32() % 8;
		INT nShift2 = Xorshift32() % 8;
		INT nShift3 = Xorshift32() % 8;

		SIZE_T dwBytes = w * h * 4;
		BYTE* pTemp = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);

		memcpy(pTemp, pRgbValues, dwBytes);

		for (INT y = 0; y < h; y++)
		{
			INT nOffset = Xorshift32() % 11;
			BOOL bLeft = Xorshift32() % 2;

			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;
				INT nOffsetIndex = (y * w + (bLeft ? max(x - nOffset, 0) : min(x + nOffset, w - 1))) * 4;

				BYTE b = pTemp[nOffsetIndex];
				BYTE g = pTemp[nOffsetIndex + 1];
				BYTE r = pTemp[nOffsetIndex + 2];

				BYTE bBlueShift = (bShift1 ? b << nShift1 : b >> nShift1);
				BYTE bGreenShift = (bShift2 ? g << nShift2 : g >> nShift2);
				BYTE bRedShift = (bShift3 ? r << nShift3 : r >> nShift3);

				BYTE bBlueOverlay = (BYTE)(bShift1 ? x + y : x - y);
				BYTE bGreenOverlay = (BYTE)(bShift2 ? x + y : x - y);
				BYTE bRedOverlay = (BYTE)(bShift3 ? x + y : x - y);

				bBlueOverlay = (BYTE)(bShift1 ? bBlueOverlay << nShift1 : bBlueOverlay >> nShift1);
				bGreenOverlay = (BYTE)(bShift2 ? bGreenOverlay << nShift2 : bGreenOverlay >> nShift2);
				bRedOverlay = (BYTE)(bShift3 ? bRedOverlay << nShift3 : bRedOverlay >> nShift3);

				pRgbValues[nIndex] = (BYTE)(bBlueOverlay & bBlueShift);
				pRgbValues[nIndex + 1] = (BYTE)(bGreenOverlay & bGreenShift);
				pRgbValues[nIndex + 2] = (BYTE)(bRedOverlay & bRedShift);
			}
		}

		HeapFree(GetProcessHeap(), 0, pTemp);

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		Sleep(50);

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload5(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nSize = 200;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateCompatibleBitmap(hdc, nSize, nSize);
		SelectObject(mhdc, hbit);

		INT rw = Xorshift32() % w;
		INT rh = Xorshift32() % h;
		INT rx = Xorshift32() % (w - rw);
		INT ry = Xorshift32() % (h - rh);

		BitBlt(hdc, rx + Xorshift32() % 5 , ry + Xorshift32() % 5, rw, rh, hdc, rx + Xorshift32() % 5, ry + Xorshift32() % 5, Xorshift32() % 2 ? SRCPAINT : SRCAND);
		DrawIcon(hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32() % 2 ? LoadIconW(NULL, IDI_ERROR) : LoadIconW(NULL, IDI_WARNING));

		BitBlt(mhdc, 0, 0, nSize, nSize, mhdc, 0, 0, Xorshift32() % 2 ? BLACKNESS : WHITENESS);
		AlphaBlend(hdc, Xorshift32() % (w - nSize), Xorshift32() % (h - nSize), nSize, nSize, mhdc, 0, 0, nSize, nSize, (BLENDFUNCTION) { 0, 0, 100, 0 });

		Sleep(10);
		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteObject(hbit);

		if (!(Xorshift32() % 75))
			InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload5(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		RGB rgb = HSLtoRGB((HSL) { fmodf((FLOAT)nTicks * 2.0f, 360.0f), 1.0f, fabsf(sinf((FLOAT)nTicks / 33.3f)) * 0.15f });

		HBRUSH hBrush = CreateHatchBrush(HS_HORIZONTAL, 0);
		SelectObject(mhdc, hBrush);
		SetBkColor(mhdc, RGB(rgb.r, rgb.g, rgb.b));

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, MERGECOPY);

		SawtoothWave(mhdc, (FLOAT)nTicks / 3.0f, 0.08f, 24.4f, 1, TRUE);
		SawtoothWave(mhdc, (FLOAT)nTicks * 2.0f, 0.08f, 20.0f, 1, TRUE);
		SineWave(mhdc, (FLOAT)nTicks * 2.0f, 0.01f, 100.0f, 100, TRUE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				pRgbValues[nIndex] = (BYTE)(b & (((x & (-y - nTicks * 4)) ^ nTicks) / 3));
				pRgbValues[nIndex + 1] = (BYTE)(g & (((x ^ (-y - nTicks * 4)) ^ nTicks) / 3));
				pRgbValues[nIndex + 2] = (BYTE)(r & (((x | (-y - nTicks * 4)) ^ nTicks) / 3));
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
		DeleteObject(hBrush);
	} lv;
}

DWORD WINAPI Payload6(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(mhdc, hbit);

		HFONT hFont = GetWingdingsFont(h / 6);
		SelectObject(mhdc, hFont);

		INT rx = Xorshift32() % w;

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		BitBlt(mhdc, rx, Xorshift32() % 10, 100, h, mhdc, rx, Xorshift32() % 10, Xorshift32() % 2 == 0 ? SRCPAINT : SRCAND);

		if (!(Xorshift32() % 5))
		{
			FLOAT fRadius = (FLOAT)(Xorshift32() % 125 + 25);
			INT x = Xorshift32() % (w - (INT)fRadius);
			INT y = Xorshift32() % (h - (INT)fRadius);
			HICON icon = Xorshift32() % 2 ? LoadIconW(NULL, IDI_ERROR) : LoadIconW(NULL, IDI_WARNING);
			for (FLOAT i = 0.0f; i < 360.0f; i++)
			{
				DrawIcon(mhdc, x + (INT)(cosf(i) * fRadius), y + (INT)(sinf(i) * fRadius), icon);
			}
		}

		SetBkMode(mhdc, TRANSPARENT);
		SetTextColor(mhdc, RANDOM_COLOR);

		TextOutW(mhdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

		BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteObject(hbit);
		DeleteObject(hFont);

		if (!(Xorshift32() % 0x4F)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload6(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, MERGECOPY);
		SawtoothWave(mhdc, (FLOAT)nTicks * 10.0f, 0.01f, 20.0f, 1, TRUE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				HSL hsl = RGBtoHSL((RGB) { r, g, b });
				RGB rgb = HSLtoRGB((HSL) { ((((FLOAT)(y / 8.0f) + _sineWave(x, 0.04f, 10.0f)) * 3.5f) + nTicks * 15.0f), fabsf(sinf((FLOAT)nTicks / 3.0f)), hsl.l });

				pRgbValues[nIndex] = rgb.b;
				pRgbValues[nIndex + 1] = rgb.g;
				pRgbValues[nIndex + 2] = rgb.r;
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload7(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		for (DWORD i = 0; i < Xorshift32() % 10 + 5; i++)
		{
			HDC hdc = GetDC(NULL);
			HDC mhdc = CreateCompatibleDC(hdc);
			HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
			SelectObject(mhdc, hbit);

			HFONT hFont = GetWingdingsFont(128);
			SelectObject(hdc, hFont);
			SetBkMode(hdc, TRANSPARENT);

			BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

			POINT points[4] = { 0 };
			points[0].x = (Xorshift32() % w) - (w / 2);
			points[0].y = (Xorshift32() % h) - (h / 2);
			points[1].x = w + ((Xorshift32() % w) - (w / 2));
			points[1].y = (Xorshift32() % h) - (h / 2);
			points[2].x = (Xorshift32() % w) - (w / 2);
			points[2].y = h + ((Xorshift32() % h) - (h / 2));
			points[3].x = w + ((Xorshift32() % w) - (w / 2));
			points[3].y = h + ((Xorshift32() % h) - (h / 2));

			PlgBlt(mhdc, points, mhdc, 0, 0, w, h, NULL, 0, 0);

			BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, Xorshift32() % 2 ? SRCPAINT : SRCAND);

			for (INT j = 0; j < 5; j++)
			{
				SetTextColor(hdc, RANDOM_COLOR);
				TextOutW(hdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);
			}

			Sleep(5);
			ReleaseDC(NULL, hdc);
			DeleteDC(mhdc);
			DeleteObject(hbit);
			DeleteObject(hFont);
		}

		InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload7(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	FLOAT fRadius = h / 10.0f;
	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		INT nRot = (INT)(sinf((FLOAT)nTicks / 10.0f) * fRadius);

		POINT points[3] = { 0 };

		points[0].x = nRot;
		points[0].y = -nRot;
		points[1].x = w + nRot;
		points[1].y = nRot;
		points[2].x = -nRot;
		points[2].y = h - nRot;

		PlgBlt(mhdc, points, hdc, 0, 0, w, h, NULL, 0, 0);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				pRgbValues[nIndex] = (BYTE)(b & ((x + (INT)(tanf((FLOAT)(y + (nTicks * 10.0f)) * 0.03f) * 25.0f) ^ y)) );
				pRgbValues[nIndex + 1] = (BYTE)(g & ((x & (-y - nTicks * 10)) ^ nTicks * 10));
				pRgbValues[nIndex + 2] = (BYTE)(r & (x ^ (y + (INT)(x * tanf((FLOAT)nTicks) + 1.0f)) + nTicks * 15));
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload8(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		HFONT hFont = GetWingdingsFont(128);
		SelectObject(hdc, hFont);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RANDOM_COLOR);
		TextOutW(hdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

		if (!(Xorshift32() % 4))
		{
			INT rx = Xorshift32() % w;
			INT ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		ReleaseDC(NULL, hdc);
		DeleteObject(hFont);

		if (!(Xorshift32() % 0x2F)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload8(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		SawtoothWave(mhdc, (FLOAT)nTicks, 0.15f, 2.0f, 1, TRUE);
		SawtoothWave(mhdc, (FLOAT)nTicks, 0.11f, 20.0f, 1, FALSE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				DWORD dwColor = 0x888888;

				HSL hsl = RGBtoHSL((RGB) { r & dwColor, g & dwColor, b & dwColor });

				FLOAT fHue = (FLOAT)((INT)hsl.h ^ y) + nTicks * 50.0f;
				FLOAT fSaturation = fabsf(sinf((FLOAT)nTicks / 10.0f));
				FLOAT fLightness = 0.5f + ((hsl.l + 0.25f - 0.5f) * fabsf(cosf((FLOAT)nTicks / 10.0f)));

				RGB rgb = HSLtoRGB((HSL) { fHue, fSaturation, fLightness });

				pRgbValues[nIndex] = rgb.b;
				pRgbValues[nIndex + 1] = rgb.g;
				pRgbValues[nIndex + 2] = rgb.r;
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload9(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		INT nSize = 200;
		INT rx = Xorshift32() % (w - nSize);
		INT ry = Xorshift32() % (h - nSize);

		BitBlt(hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, nSize, nSize, hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);

		if (!(Xorshift32() % 16))
		{
			rx = Xorshift32() % w;
			ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 0xFF)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload9(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		TriangleWave(mhdc, (FLOAT)nTicks * 20.0f, fabsf(sinf((FLOAT)nTicks / 150.0f) * 0.02f), _cosineWave((FLOAT)nTicks, 0.14f, 48.0f), Xorshift32() % 5 + 1, TRUE);
		SineWave(mhdc, (FLOAT)nTicks * 7.0f, fabsf(cosf((FLOAT)nTicks / 80.0f) * 0.05f), _cosineWave((FLOAT)nTicks, 0.11f, 72.0f), Xorshift32() % 5 + 1, TRUE);
		SawtoothWave(mhdc, (FLOAT)nTicks * 15.0f, fabsf(cosf((FLOAT)nTicks / 90.0f) * 0.15f), _cosineWave((FLOAT)nTicks, 0.18f, 32.0f), Xorshift32() % 5 + 1, TRUE);
		SquareWave(mhdc, (FLOAT)nTicks * 25.0f, fabsf(sinf((FLOAT)nTicks / 140.0f) * 0.19f), _cosineWave((FLOAT)nTicks, 0.13f, 24.0f), Xorshift32() % 5 + 1, TRUE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		BOOL bShift1 = Xorshift32() % 2;
		BOOL bShift2 = Xorshift32() % 2;
		BOOL bShift3 = Xorshift32() % 2;

		INT nShift1 = Xorshift32() % 8;
		INT nShift2 = Xorshift32() % 8;
		INT nShift3 = Xorshift32() % 8;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				BYTE bBlueShift = (bShift1 ? b << nShift1 : b >> nShift1);
				BYTE bGreenShift = (bShift2 ? g << nShift2 : g >> nShift2);
				BYTE bRedShift = (bShift3 ? r << nShift3 : r >> nShift3);

				HSL srcHsl = RGBtoHSL((RGB) { r, g, b });
				HSL hsl = RGBtoHSL((RGB) { (BYTE)(r ^ bBlueShift), (BYTE)(g ^ bBlueShift), (BYTE)(b ^ bBlueShift) });
				RGB rgb = HSLtoRGB((HSL) { (FLOAT)((INT)hsl.h ^ (INT)srcHsl.h) + (FLOAT)nTicks * 20.0f, hsl.s, (FLOAT)(BYTE)(bBlueShift ^ bGreenShift ^ bRedShift) / 255.0f });

				pRgbValues[nIndex] = rgb.b;
				pRgbValues[nIndex + 1] = rgb.g;
				pRgbValues[nIndex + 2] = rgb.r;
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload10(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nSize = 200;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateCompatibleBitmap(hdc, nSize, nSize);
		SelectObject(mhdc, hbit);

		INT rx = Xorshift32() % (w - nSize);
		INT ry = Xorshift32() % (h - nSize);

		BitBlt(hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, nSize, nSize, hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);

		if (!(Xorshift32() % 16))
		{
			rx = Xorshift32() % w;
			ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		if (!(Xorshift32() % 8))
		{
			BitBlt(mhdc, 0, 0, nSize, nSize, mhdc, 0, 0, Xorshift32() % 2 ? BLACKNESS : WHITENESS);
			AlphaBlend(hdc, Xorshift32() % (w - nSize), Xorshift32() % (h - nSize), nSize, nSize, mhdc, 0, 0, nSize, nSize, (BLENDFUNCTION) { 0, 0, 100, 0 });
		}

		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 0xFF)) InvalidateRect(NULL, NULL, TRUE);

		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI LayeredPayload10(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		TriangleWave(mhdc, (FLOAT)nTicks * 20.0f, 0.02f, 30.0f, 1, TRUE);
		SquareWave(mhdc, (FLOAT)nTicks * 2.0f, 0.35f, 10.0f, 1, FALSE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				DWORD dwColor = 0x777777;

				HSL hsl = RGBtoHSL((RGB) { r & dwColor, g & dwColor, b & dwColor });

				FLOAT fAndFractal = (FLOAT)(BYTE)((x + (INT)((y * tanf((FLOAT)nTicks / 4.0f))) & (y + nTicks * 20)) + nTicks * 10);
				FLOAT fLightness = (hsl.l * fmodf(fabsf(sinf((FLOAT)nTicks / 16.0f) * 4.0f), 3.1f)) * (fAndFractal / 255.0f / 3.0f + 0.66f);

				RGB rgb = HSLtoRGB((HSL) { 0.0f, 0.0f, max(min(fLightness, 1.0f), 0.0f)});

				pRgbValues[nIndex] = rgb.b;
				pRgbValues[nIndex + 1] = rgb.g;
				pRgbValues[nIndex + 2] = rgb.r;
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload11(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		for (INT i = 0; i < 5; i++)
		{
			HFONT hFont = GetWingdingsFont(128);
			SelectObject(hdc, hFont);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RANDOM_COLOR);

			TextOutW(hdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

			DeleteObject(hFont);
		}

		INT rx = Xorshift32() % w;
		INT ry = Xorshift32() % h;
		INT rw = Xorshift32() % (w - rx);
		INT rh = Xorshift32() % (h - ry);

		BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());

		Sleep(5);

		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 0x7F)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload11(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		BitBlt(mhdc, Xorshift32() % w, Xorshift32() % h, w, h, hdc, Xorshift32() % w, Xorshift32() % h, SRCCOPY);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE bColor = (BYTE)fabsf(_sineWave((FLOAT)nTicks, 0.01f, 128.0f));

				BYTE b = pRgbValues[nIndex] & bColor;
				BYTE g = pRgbValues[nIndex + 1] & bColor;
				BYTE r = pRgbValues[nIndex + 2] & bColor;

				pRgbValues[nIndex] = (BYTE)(b & (((255 - (y - nTicks * 10)) ^ nTicks)));
				pRgbValues[nIndex + 1] = (BYTE)(g & (((y - nTicks * 8) ^ nTicks)));
				pRgbValues[nIndex + 2] = (BYTE)(r & ((((x + nTicks * 6) + (y + nTicks * 6)) ^ nTicks)));
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		Sleep(50);

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload12(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	HBITMAP hbit = CreateCompatibleBitmap(GetDC(NULL), w, h);
	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HDC mhdc2 = CreateCompatibleDC(hdc);
		HBITMAP hbit2 = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(mhdc, hbit);
		SelectObject(mhdc2, hbit2);

		AlphaBlend(mhdc, 0, 0, w, h, mhdc2, 0, 0, w, h, (BLENDFUNCTION) { 0, 0, 20, 0 });

		HFONT hFont = GetWingdingsFont(96);
		SelectObject(mhdc, hFont);

		SetBkMode(mhdc, TRANSPARENT);
		SetTextColor(mhdc, RGB(255, 0, 0));

		TextOutW(mhdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

		BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		Sleep(5);

		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteDC(mhdc2);
		DeleteObject(hbit2);
		DeleteObject(hFont);
	} lv;
}

DWORD WINAPI LayeredPayload12(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				HSL hsl = RGBtoHSL((RGB) { r, g, b });
				RGB rgb = HSLtoRGB((HSL) { ScaleToHueRange((FLOAT)(x ^ y + nTicks * 20)), 1.0f, hsl.l });

				pRgbValues[nIndex] = rgb.b;
				pRgbValues[nIndex + 1] = rgb.g;
				pRgbValues[nIndex + 2] = rgb.r;
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		Sleep(50);

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload13(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		INT nSize = 200;
		INT rx = Xorshift32() % (w - nSize);
		INT ry = Xorshift32() % (h - nSize);

		BitBlt(hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, nSize, nSize, hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);

		if (!(Xorshift32() % 16))
		{
			rx = Xorshift32() % w;
			ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 0xFF)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload13(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	BOOL bGradientDirection = Xorshift32() % 2;
	INT nGradientShift = Xorshift32() % 17;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		INT nWidth = (INT)(w + fabsf(_sineWave((FLOAT)nTicks, 0.05f, (FLOAT)w * 5.0f))) + 1;
		StretchBlt(mhdc, 0, 0, nWidth, h, hdc, 0, 0, w, h, SRCCOPY);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		RGB rgb = HSLtoRGB((HSL) { (FLOAT)nTicks * 2.0f, 1.0f, 0.5f });

		if (!(nTicks % 4))
		{
			bGradientDirection = Xorshift32() % 2;
			nGradientShift = Xorshift32() % 6;
		}

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				BYTE bGradient = (BYTE)((bGradientDirection ? x + y : x - y) + nTicks * 20);
				bGradient = bGradient >> nGradientShift;

				pRgbValues[nIndex] = (b | rgb.b | bGradient);
				pRgbValues[nIndex + 1] = (g | rgb.g | bGradient);
				pRgbValues[nIndex + 2] = (r | rgb.r | bGradient);
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		Sleep(20);

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload14(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nSize = 200;
	INT nShift = 50;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, h - nShift, w, h, hdc, 0, 0, SRCCOPY);
		BitBlt(mhdc, 0, 0, w, h, hdc, 0, nShift, SRCCOPY);
		BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		for (INT i = 0; i < 5; i++)
		{
			INT rx = Xorshift32() % (w - nSize);
			INT ry = Xorshift32() % (h - nSize);

			BitBlt(hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, nSize, nSize, hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);
		}

		if (Xorshift32() % 2)
		{
			INT rx = Xorshift32() % w;
			INT ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		Sleep(20);

		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteObject(hbit);

		if (!(Xorshift32() % 0x1F)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload14(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		SineWave(mhdc, (FLOAT)nTicks * 20.0f, 0.02f, _sineWave((FLOAT)nTicks, 0.07f, 50.0f), 1, TRUE);
		SineWave(mhdc, (FLOAT)nTicks * 20.0f, 0.02f, _cosineWave((FLOAT)nTicks, 0.07f, 50.0f), 1, FALSE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		BOOL bShift1 = Xorshift32() % 2;
		BOOL bShift2 = Xorshift32() % 2;

		INT nShift1 = Xorshift32() % 8;
		INT nShift2 = Xorshift32() % 2;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				HSL hsl = RGBtoHSL((RGB) { r, g, b });

				INT nBaseSL = (INT)(hsl.s * 100.0f) ^ (INT)(hsl.l * 100.0f);
				INT nShiftS = bShift2 ? (INT)(hsl.s * 100.0f) << nShift2 : (INT)(hsl.s * 100.0f) >> nShift2;

				FLOAT fHue = (FLOAT)(bShift1 ? (INT)hsl.h << nShift1 : (INT)hsl.h >> nShift1);
				FLOAT fSaturation = (FLOAT)(nBaseSL ^ nShiftS | (nTicks * 20)) / 100.0f;
				FLOAT fLightness = fmodf((FLOAT)((INT)(sinf((FLOAT)nTicks / 15.0f) * tanf((FLOAT)nTicks) * 100.0f) & (INT)(hsl.l * 100.0f)) / 100.0f, 1.0f);

				RGB rgb = HSLtoRGB((HSL) { fHue, fSaturation, fLightness });

				pRgbValues[nIndex] = (BYTE)(rgb.b);
				pRgbValues[nIndex + 1] = (BYTE)(rgb.g);
				pRgbValues[nIndex + 2] = (BYTE)(rgb.r);
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload15(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nSize = 200;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(mhdc, hbit);

		switch (Xorshift32() % 4)
		{
		case 0:
		{
			INT rx = Xorshift32() % (w / 2);
			INT ry = Xorshift32() % (h / 2);

			BitBlt(hdc, rx, ry, nSize, nSize, hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);
			break;
		}
		case 1:
		{
			INT rx = Xorshift32() % (w / 2);
			INT ry = (h / 2) + Xorshift32() % (h / 2);

			BitBlt(hdc, rx, ry + Xorshift32() % 10, nSize, nSize, hdc, rx + Xorshift32() % 10, ry, Xorshift32() % 2 ? SRCPAINT : SRCAND);
			break;
		}
		case 2:
		{
			INT rx = (w / 2) + Xorshift32() % (w / 2);
			INT ry = Xorshift32() % (h / 2);

			BitBlt(hdc, rx + Xorshift32() % 10, ry, nSize, nSize, hdc, rx, ry + Xorshift32() % 10, Xorshift32() % 2 ? SRCPAINT : SRCAND);
			break;
		}
		case 3:
		{
			INT rx = (w / 2) + Xorshift32() % (w / 2);
			INT ry = (h / 2) + Xorshift32() % (h / 2);

			BitBlt(hdc, rx + Xorshift32() % 10, ry + Xorshift32() % 10, nSize, nSize, hdc, rx, ry, Xorshift32() % 2 ? SRCPAINT : SRCAND);
			break;
		}
		}

		if (!(Xorshift32() % 16))
		{
			INT rx = Xorshift32() % w;
			INT ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		if (!(Xorshift32() % 16))
		{
			BitBlt(mhdc, 0, 0, nSize, nSize, mhdc, 0, 0, Xorshift32() % 2 ? BLACKNESS : WHITENESS);
			AlphaBlend(hdc, Xorshift32() % (w - nSize), Xorshift32() % (h - nSize), nSize, nSize, mhdc, 0, 0, nSize, nSize, (BLENDFUNCTION) { 0, 0, 100, 0 });
		}

		if (!(Xorshift32() % 8))
		{
			HFONT hFont = GetWingdingsFont(128);
			SelectObject(hdc, hFont);

			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RANDOM_COLOR);

			TextOutW(hdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

			DeleteObject(hFont);
		}

		ReleaseDC(NULL, hdc);
		DeleteDC(mhdc);
		DeleteObject(hbit);

		if (!(Xorshift32() % 0xFF)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload15(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		SineWave(mhdc, (FLOAT)nTicks * 10.0f, 0.04f, _sineWave((FLOAT)nTicks, 0.09f, 40.0f), 1, TRUE);
		SineWave(mhdc, (FLOAT)nTicks * 10.0f, 0.04f, _cosineWave((FLOAT)nTicks, 0.09f, 40.0f), 1, FALSE);

		SineWave(mhdc, (FLOAT)-nTicks * 60.0f, 0.005f, _sineWave((FLOAT)nTicks, 0.09f, 50.0f), 1, TRUE);
		SineWave(mhdc, (FLOAT)-nTicks * 60.0f, 0.005f, _cosineWave((FLOAT)nTicks, 0.09f, 50.0f), 1, FALSE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		BYTE bColor[3];
		for (INT i = 0; i < 3; i++)
			bColor[i] = (BYTE)(Xorshift32());

		INT nOperations[3];
		for (INT i = 0; i < 3; i++)
			nOperations[i] = Xorshift32() % 7;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				for (INT i = 0; i < 3; i++)
				{
					BYTE bSrcColor = pRgbValues[nIndex + i];

					switch (nOperations[i])
					{
					case 0:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor + bColor[i]);
						break;
					case 1:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor ^ bColor[i]);
						break;
					case 2:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor & bColor[i]);
						break;
					case 3:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor | bColor[i]);
						break;
					case 4:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor * bColor[i]);
						break;
					case 5:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor >> (bColor[i] % 8));
						break;
					case 6:
						pRgbValues[nIndex + i] = (BYTE)(bSrcColor << (bColor[i] % 8));
						break;
					}
				}
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload16(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HBRUSH hBrush = CreateSolidBrush(RANDOM_COLOR);
		SelectObject(hdc, hBrush);

		PatBlt(hdc, 0, 0, w, h, PATINVERT);

		if (!(Xorshift32() % 4))
		{
			INT rx = Xorshift32() % w;
			INT ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		Sleep(20);

		ReleaseDC(NULL, hdc);
		DeleteObject(hBrush);
	} lv;
}

DWORD WINAPI LayeredPayload16(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		for (INT i = 0; i < w || i < h; i += (nTicks + 2))
		{
			BitBlt(mhdc, 0, 0, w, h, hdc, i, i, SRCCOPY);
		}

		TriangleWave(mhdc, (FLOAT)nTicks, 0.3f, 5.0f, 1, FALSE);
		SineWave(mhdc, (FLOAT)nTicks * 8.0f, 0.02f, 6.0f, 1, FALSE);

		BYTE* pRgbValues = (BYTE*)ppvBits;
		
		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];
				
				pRgbValues[nIndex] = (BYTE)((((b ^ (r >> (3 + nTicks % 5)))) << (nTicks % 8)) & ((-(INT)((FLOAT)x + _sineWave((FLOAT)nTicks, 0.1f, (FLOAT)w / 5.0f)) & (y + nTicks * 10)) ^ nTicks * 5));
				pRgbValues[nIndex + 1] = (BYTE)((((g ^ (g << (5 + nTicks % 5)))) >> (nTicks % 8)) & (((INT)((FLOAT)x + _cosineWave((FLOAT)nTicks, 0.1f, (FLOAT)w / 5.0f)) | (y - nTicks * 15)) ^ nTicks * 5));
				pRgbValues[nIndex + 2] = (BYTE)((((r ^ (b >> (4 + nTicks % 5)))) << (nTicks % 8)) & ((-(INT)((FLOAT)x + _sineWave((FLOAT)nTicks, 0.1f, (FLOAT)w / 5.0f)) ^ (-y + nTicks * 8)) ^ nTicks * 5));
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload17(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nScale = h / 5;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HBRUSH hBrush = CreateSolidBrush(RANDOM_COLOR);
		SelectObject(hdc, hBrush);

		for (INT i = 0; i < 30; i++)
		{
			INT rx = Xorshift32() % (w - nScale);
			INT ry = Xorshift32() % (h - nScale);

			BitBlt(hdc, rx + Xorshift32() % 5, ry + Xorshift32() % 5, nScale, nScale, hdc, rx + Xorshift32() % 5, ry + Xorshift32() % 5, Xorshift32() % 2 ? SRCAND : SRCPAINT);
		}

		for (INT i = 0; i < 3; i++)
		{
			INT rx = Xorshift32() % w;
			INT ry = Xorshift32() % h;
			INT rw = Xorshift32() % (w - rx);
			INT rh = Xorshift32() % (h - ry);

			BitBlt(hdc, rx, ry, rw, rh, hdc, Xorshift32() % w, Xorshift32() % h, Xorshift32());
		}

		Sleep(1);

		ReleaseDC(NULL, hdc);
		DeleteObject(hBrush);

		if (!(Xorshift32() % 16)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload17(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nHalfScale = 8;
	INT nTicks = 0;

	for (;;)
	{
		INT nScale = nHalfScale + (INT)((_sineWave((FLOAT)nTicks, 0.1f, 1.0f) + 1.0f) / 2.0f * (FLOAT)nHalfScale);

		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		SetStretchBltMode(mhdc, HALFTONE);

		StretchBlt(mhdc, 0, 0, w / nScale, h / nScale, hdc, 0, 0, w, h, SRCCOPY);
		StretchBlt(mhdc, 0, 0, w, h, mhdc, 0, 0, w / nScale, h / nScale, SRCCOPY);

		SawtoothWave(mhdc, 0.0f, 0.2f, _cosineWave((FLOAT)nTicks, 0.2f, (FLOAT)h / 35.0f), 1, TRUE);
		SawtoothWave(mhdc, 0.0f, 0.2f, _sineWave((FLOAT)nTicks, 0.2f, (FLOAT)h / 35.0f), 1, FALSE);

		SineWave(mhdc, (FLOAT)nTicks, 0.007f, _sineWave((FLOAT)nTicks, 0.30f, (FLOAT)h / 10.0f), 1, TRUE);
		SineWave(mhdc, (FLOAT)nTicks, 0.007f, _cosineWave((FLOAT)nTicks, 0.30f, (FLOAT)h / 10.0f), 1, FALSE);

		RGB rgb = HSLtoRGB((HSL) { (FLOAT)nTicks / 36.0f, fabsf(_sineWave((FLOAT)nTicks, fabsf(sinf((FLOAT)nTicks)), 1.0f)), fabsf(_sineWave((FLOAT)nTicks, 0.5f, 1.0f)) });
		INT nDarkColor = (INT)fabsf(_sineWave((FLOAT)nTicks, 0.05f, (FLOAT)0xAF));

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				INT nLines = ((x + (INT)((FLOAT)y * _sineWave((FLOAT)nTicks, 0.07f, 2.0f))));

				pRgbValues[nIndex] = (BYTE)((b ^ (b >> (rgb.r / 50) << (nTicks % 6 + 3))) & nLines);
				pRgbValues[nIndex + 1] = (BYTE)((g ^ (g << (rgb.g / 50) >> (nTicks % 7 + 1))) & nLines);
				pRgbValues[nIndex + 2] = (BYTE)((r ^ (r >> (rgb.b / 50) << (nTicks % 4 + 4))) & nLines);
			}
		}

		HBRUSH hBrush = CreateSolidBrush(RGB(nDarkColor, nDarkColor, nDarkColor));

		HDC hdcWnd = GetDC(g_hWnd);
		SelectObject(hdcWnd, hBrush);

		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, MERGECOPY);

		DeleteObject(hBrush);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload18(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nScale = h / 5;
	INT nZoom = 20;

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		for (INT i = 0; i < 10; i++)
		{
			INT rx = Xorshift32() % (w - nScale);
			INT ry = Xorshift32() % (h - nScale);

			if (Xorshift32() % 2)
				StretchBlt(hdc, rx - nZoom / 2, ry - nZoom / 2, nScale + nZoom, nScale + nZoom, hdc, rx, ry, nScale, nScale, Xorshift32() % 2 ? SRCPAINT : SRCAND);
			else
				StretchBlt(hdc, rx + nZoom / 2, ry + nZoom / 2, nScale - nZoom, nScale - nZoom, hdc, rx, ry, nScale, nScale, Xorshift32() % 2 ? SRCPAINT : SRCAND);
		}

		Sleep(1);
		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 32)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload18(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		DWORD dwSize = w * h * 4;
		BYTE* pTemp = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		SineWave(mhdc, (FLOAT)nTicks * 8.0f, 0.02f, (FLOAT)w * sinf((FLOAT)nTicks / 20.0f), 1, TRUE);
		memcpy(pTemp, ppvBits, dwSize);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);
		SineWave(mhdc, (FLOAT)nTicks * 8.0f, 0.02f, (FLOAT)h * cosf((FLOAT)nTicks / 20.0f), 1, FALSE);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE bColor = 0x77;

				BYTE b = (BYTE)((pRgbValues[nIndex + 0] + pTemp[nIndex + 0]) / 2);
				BYTE g = (BYTE)((pRgbValues[nIndex + 1] + pTemp[nIndex + 1]) / 2);
				BYTE r = (BYTE)((pRgbValues[nIndex + 2] + pTemp[nIndex + 2]) / 2);

				b &= bColor;
				g &= bColor;
				r &= bColor;

				FLOAT fHue = 0;

				fHue += _sineWave((FLOAT)(x - y), 0.008f, 1.0f) * sinf((FLOAT)nTicks * 0.006f);
				fHue += _sineWave((FLOAT)x, 0.01f, 1.0f) * cosf((FLOAT)nTicks * 0.03f);
				fHue += _sineWave((FLOAT)(x + x + y), 0.004f, 1.0f) * sinf((FLOAT)nTicks * 0.05f);
				fHue += _sineWave((FLOAT)y, 0.014f, 1.0f) * cosf((FLOAT)nTicks * 0.03f);

				fHue = ((fHue / 4.0f + 1.0f) / 2.0f) * 360.0f;

				HSL hsl = RGBtoHSL((RGB) { r, g, b });
				RGB rgb = HSLtoRGB((HSL) { hsl.h + fHue + hsl.s * 360.0f + hsl.l * 360.0f + (FLOAT)nTicks * 10.0f, 1.0f, 0.5f });

				pRgbValues[nIndex] = rgb.b;
				pRgbValues[nIndex + 1] = rgb.g;
				pRgbValues[nIndex + 2] = rgb.r;
			}
		}

		HeapFree(GetProcessHeap(), 0, pTemp);

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload19(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT nSize = w / 24;

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		for (INT x = 0; x < w; x += nSize)
		{
			for (INT y = 0; y < h; y += nSize)
			{
				BitBlt(hdc, x + Xorshift32() % 20, y + Xorshift32() % 20, nSize, nSize, hdc, x + Xorshift32() % 20, y + Xorshift32() % 20, Xorshift32() % 2 ? SRCPAINT : SRCAND);
			}
		}

		Sleep(1);
		ReleaseDC(NULL, hdc);

		if (!(Xorshift32() % 16)) InvalidateRect(NULL, NULL, TRUE);
	} lv;
}

DWORD WINAPI LayeredPayload19(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		INT nScreens = 1 + (INT)fabsf(_sineWave((FLOAT)nTicks, 0.05f, 19.0f));

		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		SelectObject(mhdc, hbit);

		BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

		SawtoothWave(mhdc, (FLOAT)nTicks * 4.0f, 0.1f, 20.0f, 1, TRUE);
		TriangleWave(mhdc, (FLOAT)nTicks * 4.0f, 0.05f, _sineWave((FLOAT)nTicks, 0.2f, 20.0f), 1, FALSE);

		for (INT i = 0; i < w; i += w / nScreens)
			BitBlt(mhdc, i, 0, w / nScreens, h, mhdc, 0, 0, SRCCOPY);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE bInvertColor = (BYTE)(nTicks / 32 % 2 * 0xFF);

				BYTE b = min(pRgbValues[nIndex + 0] / 32 * 32, 255) ^ bInvertColor;
				BYTE g = min(pRgbValues[nIndex + 1] / 32 * 32, 255) ^ bInvertColor;
				BYTE r = min(pRgbValues[nIndex + 2] / 32 * 32, 255) ^ bInvertColor;

				FLOAT fHue = 0;

				fHue += _sineWave((FLOAT)(x - y), 0.01f, 1.0f) * sinf((FLOAT)nTicks * 0.08f);
				fHue += _sineWave((FLOAT)x, 0.02f, 1.0f) * cosf((FLOAT)nTicks * 0.02f);
				fHue += _sineWave((FLOAT)(x + x + y), 0.007f, 1.0f) * sinf((FLOAT)nTicks * 0.02f);
				fHue += _sineWave((FLOAT)y, 0.008f, 1.0f) * cosf((FLOAT)nTicks * 0.15f);

				fHue = ((fHue / 4.0f + 1.0f) / 2.0f) * 360.0f;
				
				RGB rgb = HSLtoRGB((HSL) { fHue + (FLOAT)nTicks * 10.0f, 1.0f, 0.5f });

				pRgbValues[nIndex] = (BYTE)(b & (rgb.b | ((x ^ (y + nTicks * 10)) + nTicks * 5)));
				pRgbValues[nIndex + 1] = (BYTE)(g & (rgb.g | ((x ^ (y - nTicks * 10)) + nTicks * 5)));
				pRgbValues[nIndex + 2] = (BYTE)(r & rgb.r);
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteObject(hbit);
	} lv;
}

DWORD WINAPI Payload20(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	for (;;)
	{
		HDC hdc = GetDC(NULL);

		HFONT hFont = GetWingdingsFont(128);
		SelectObject(hdc, hFont);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RANDOM_COLOR);
		TextOutW(hdc, Xorshift32() % w, Xorshift32() % h, L"ÿ", 1);

		ReleaseDC(NULL, hdc);
		DeleteObject(hFont);

		Sleep(Xorshift32() % 50);
	} lv;
}

DWORD WINAPI LayeredPayload20(LPVOID lv)
{
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	BITMAPINFO bmpInfo = { 0 };
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = w;
	bmpInfo.bmiHeader.biHeight = -h;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	VOID* ppvBits = NULL;

	INT nTicks = 0;

	for (;;)
	{
		HDC hdc = GetDC(NULL);
		HDC mhdc = CreateCompatibleDC(hdc);
		HDC mhdc2 = CreateCompatibleDC(hdc);
		HDC mhdc3 = CreateCompatibleDC(hdc);
		HBITMAP hbit = CreateDIBSection(mhdc, &bmpInfo, BI_RGB, &ppvBits, NULL, 0);
		HBITMAP hbit2 = CreateCompatibleBitmap(hdc, w, h);
		HBITMAP hbit3 = CreateCompatibleBitmap(hdc, w, h);
		SelectObject(mhdc, hbit);
		SelectObject(mhdc2, hbit2);
		SelectObject(mhdc3, hbit3);

		INT nWidth = w / 2 + (INT)((_cosineWave((FLOAT)nTicks, 0.04f, 0.5f) + 0.5f) * ((FLOAT)w * 4.0f));
		INT nHeight = h / 2 + (INT)((_sineWave((FLOAT)nTicks, 0.04f, 0.5f) + 0.5f) * ((FLOAT)h * 2.0f));

		StretchBlt(mhdc3, 0, 0, w / 2, h, hdc, 0, 0, w, h, SRCCOPY);
		StretchBlt(mhdc3, w, 0, -(w / 2), h, hdc, 0, 0, w, h, SRCCOPY);

		StretchBlt(mhdc2, 0, 0, nWidth, h, mhdc3, 0, 0, w, h, SRCCOPY);
		StretchBlt(mhdc2, w, 0, -(w / 2), h, mhdc2, 0, 0, w / 2, h, SRCCOPY);

		StretchBlt(mhdc, 0, -(nHeight - h), w, nHeight, mhdc2, 0, 0, w, h, SRCCOPY);
		StretchBlt(mhdc, 0, -(nHeight - h), w, -nHeight, mhdc2, 0, 0, w, h, SRCCOPY);

		BYTE* pRgbValues = (BYTE*)ppvBits;

		for (INT y = 0; y < h; y++)
		{
			for (INT x = 0; x < w; x++)
			{
				INT nIndex = (y * w + x) * 4;

				BYTE b = pRgbValues[nIndex + 0];
				BYTE g = pRgbValues[nIndex + 1];
				BYTE r = pRgbValues[nIndex + 2];

				BYTE bFormula = (BYTE)(((x ^ (y + nTicks * 20)) ^ (x & (y + nTicks * 2)) ^ (x | (y + nTicks * 7))) | (((x + y + nTicks * 20) >> 1) >> 3));

				pRgbValues[nIndex] = (BYTE)(b ^ bFormula);
				pRgbValues[nIndex + 1] = (BYTE)(g ^ bFormula);
				pRgbValues[nIndex + 2] = (BYTE)(r ^ bFormula);
			}
		}

		HDC hdcWnd = GetDC(g_hWnd);
		BitBlt(hdcWnd, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

		nTicks++;

		ReleaseDC(NULL, hdc);
		ReleaseDC(g_hWnd, hdcWnd);
		DeleteDC(mhdc);
		DeleteDC(mhdc2);
		DeleteDC(mhdc3);
		DeleteObject(hbit);
		DeleteObject(hbit2);
		DeleteObject(hbit3);
	} lv;
}

VOID SineWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical)
{
	HDC mhdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mhdc, hbit);
	BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

	if (bVertical)
	{
		for (INT i = 0; i < h; i += nBlockSize)
		{
			INT nOffset = (INT)_sineWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, -w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
		}
	}
	else
	{
		for (INT i = 0; i < w; i += nBlockSize)
		{
			INT nOffset = (INT)_sineWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, i, -h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
		}
	}

	BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

	DeleteDC(mhdc);
	DeleteObject(hbit);
}

VOID SawtoothWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical)
{
	HDC mhdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mhdc, hbit);
	BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

	if (bVertical)
	{
		for (INT i = 0; i < h; i += nBlockSize)
		{
			INT nOffset = (INT)_sawtoothWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, -w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
		}
	}
	else
	{
		for (INT i = 0; i < w; i += nBlockSize)
		{
			INT nOffset = (INT)_sawtoothWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, i, -h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
		}
	}

	BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

	DeleteDC(mhdc);
	DeleteObject(hbit);
}

VOID TanWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical)
{
	HDC mhdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mhdc, hbit);
	BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

	if (bVertical)
	{
		for (INT i = 0; i < h; i += nBlockSize)
		{
			INT nOffset = (INT)_tanWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, -w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
		}
	}
	else
	{
		for (INT i = 0; i < w; i += nBlockSize)
		{
			INT nOffset = (INT)_tanWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, i, -h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
		}
	}

	BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

	DeleteDC(mhdc);
	DeleteObject(hbit);
}

VOID TriangleWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical)
{
	HDC mhdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mhdc, hbit);
	BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

	if (bVertical)
	{
		for (INT i = 0; i < h; i += nBlockSize)
		{
			INT nOffset = (INT)_triangleWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, -w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
		}
	}
	else
	{
		for (INT i = 0; i < w; i += nBlockSize)
		{
			INT nOffset = (INT)_triangleWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, i, -h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
		}
	}

	BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

	DeleteDC(mhdc);
	DeleteObject(hbit);
}

VOID SquareWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical)
{
	HDC mhdc = CreateCompatibleDC(hdc);
	HBITMAP hbit = CreateCompatibleBitmap(hdc, w, h);
	SelectObject(mhdc, hbit);
	BitBlt(mhdc, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

	if (bVertical)
	{
		for (INT i = 0; i < h; i += nBlockSize)
		{
			INT nOffset = (INT)_squareWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, -w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
			BitBlt(mhdc, w + nOffset, i, w, nBlockSize, hdc, 0, i, SRCCOPY);
		}
	}
	else
	{
		for (INT i = 0; i < w; i += nBlockSize)
		{
			INT nOffset = (INT)_squareWave(i + fAngle, fFreq, fSize);

			BitBlt(mhdc, i, -h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
			BitBlt(mhdc, i, h + nOffset, nBlockSize, h, hdc, i, 0, SRCCOPY);
		}
	}

	BitBlt(hdc, 0, 0, w, h, mhdc, 0, 0, SRCCOPY);

	DeleteDC(mhdc);
	DeleteObject(hbit);
}

DWORD WINAPI GdiOverlayWindowThread(LPVOID lv)
{
	if (!g_bLayeredGdiCompatible) return GdiOverlayFallbackWindow();

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"ScreensaverShaderWnd";
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = LayeredWndProc;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hInstance = g_hInstance;
	wc.hIcon = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassW(&wc)) return 0;

	g_hWnd = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
		L"ScreensaverShaderWnd", L"screensaver.exe", WS_POPUP,
		0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN),
		NULL, NULL, g_hInstance, NULL);

	if (g_hWnd == NULL) return 0;

	ShowWindow(g_hWnd, SW_SHOW);

	SetLayeredWindowAttributes(g_hWnd, 0x000000, 255, LWA_ALPHA);
	Windows10LayeredWindowSupport(g_hWnd);

	SetTimer(g_hWnd, 1, 5, UpdateTimerProc);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0; lv;
}

LRESULT CALLBACK LayeredWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_WINDOWPOSCHANGING:
	{
		WINDOWPOS* wp = (WINDOWPOS*)lParam;
		wp->hwndInsertAfter = HWND_TOPMOST;
		wp->x = 0;
		wp->y = 0;
		wp->cx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		wp->cy = GetSystemMetrics(SM_CYVIRTUALSCREEN);
		wp->flags &= ~SWP_NOZORDER;
		return 0;
	}
	case WM_ERASEBKGND:
		return 1;
	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}
}

VOID CALLBACK UpdateTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN), SWP_NOACTIVATE);
	hWnd; uMsg; idEvent; dwTime;
}