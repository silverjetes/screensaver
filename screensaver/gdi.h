#pragma once
#include "screensaver.h"

typedef struct _GDI_PAYLOAD
{
	PTHREAD_START_ROUTINE fnNormal;
	PTHREAD_START_ROUTINE fnLayered;
} GDI_PAYLOAD;

#define ScaleToHueRange(x) ((x) / 255.0f * 360.0f)
#define RANDOM_COLOR (Xorshift32() % (0xFFFFFF + 1))

extern HWND g_hWnd;
extern GDI_PAYLOAD gdiPayloads[20];

DWORD WINAPI GdiOverlayWindowThread(LPVOID lv);

LRESULT CALLBACK LayeredWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID CALLBACK UpdateTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

VOID InitializeGdiPayloads();
DWORD WINAPI GdiThread(LPVOID lv);
VOID ExecutePayload(INT nIndex, DWORD dwDuration);

DWORD WINAPI Payload1(LPVOID lv);
DWORD WINAPI LayeredPayload1(LPVOID lv);
DWORD WINAPI Payload2(LPVOID lv);
DWORD WINAPI LayeredPayload2(LPVOID lv);
DWORD WINAPI Payload3(LPVOID lv);
DWORD WINAPI LayeredPayload3(LPVOID lv);
DWORD WINAPI Payload4(LPVOID lv);
DWORD WINAPI LayeredPayload4(LPVOID lv);
DWORD WINAPI Payload5(LPVOID lv);
DWORD WINAPI LayeredPayload5(LPVOID lv);
DWORD WINAPI Payload6(LPVOID lv);
DWORD WINAPI LayeredPayload6(LPVOID lv);
DWORD WINAPI Payload7(LPVOID lv);
DWORD WINAPI LayeredPayload7(LPVOID lv);
DWORD WINAPI Payload8(LPVOID lv);
DWORD WINAPI LayeredPayload8(LPVOID lv);
DWORD WINAPI Payload9(LPVOID lv);
DWORD WINAPI LayeredPayload9(LPVOID lv);
DWORD WINAPI Payload10(LPVOID lv);
DWORD WINAPI LayeredPayload10(LPVOID lv);
DWORD WINAPI Payload11(LPVOID lv);
DWORD WINAPI LayeredPayload11(LPVOID lv);
DWORD WINAPI Payload12(LPVOID lv);
DWORD WINAPI LayeredPayload12(LPVOID lv);
DWORD WINAPI Payload13(LPVOID lv);
DWORD WINAPI LayeredPayload13(LPVOID lv);
DWORD WINAPI Payload14(LPVOID lv);
DWORD WINAPI LayeredPayload14(LPVOID lv);
DWORD WINAPI Payload15(LPVOID lv);
DWORD WINAPI LayeredPayload15(LPVOID lv);
DWORD WINAPI Payload16(LPVOID lv);
DWORD WINAPI LayeredPayload16(LPVOID lv);
DWORD WINAPI Payload17(LPVOID lv);
DWORD WINAPI LayeredPayload17(LPVOID lv);
DWORD WINAPI Payload18(LPVOID lv);
DWORD WINAPI LayeredPayload18(LPVOID lv);
DWORD WINAPI Payload19(LPVOID lv);
DWORD WINAPI LayeredPayload19(LPVOID lv);
DWORD WINAPI Payload20(LPVOID lv);
DWORD WINAPI LayeredPayload20(LPVOID lv);

VOID SineWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical);
VOID SawtoothWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical);
VOID TanWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical);
VOID TriangleWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical);
VOID SquareWave(HDC hdc, FLOAT fAngle, FLOAT fFreq, FLOAT fSize, INT nBlockSize, BOOL bVertical);