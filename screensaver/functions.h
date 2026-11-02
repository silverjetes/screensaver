#pragma once
#include "screensaver.h"

typedef NTSTATUS(NTAPI* RtlGetVersion_pdef) (
	_Out_ PRTL_OSVERSIONINFOW lpVersionInformation
);

typedef HRESULT(*DwmEnableComposition_pdef) (
	_In_ UINT uCompositionAction
);

typedef BOOL(*SetWindowDisplayAffinity_pdef) (
	_In_ HWND  hWnd,
	_In_ DWORD dwAffinity
);

#define QuickCreateThread(lpStartAddress, lpParameter) CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)lpStartAddress, lpParameter, 0, NULL)

#define _sec_ms(nSecs) (nSecs * 1000)
#define _min_ms(nMins) _sec_ms((nMins * 60))

#define _sineWave(fAngle, fFreq, fSize) (sinf((fAngle) * (fFreq)) * (fSize))
#define _cosineWave(fAngle, fFreq, fSize) (cosf((fAngle) * (fFreq)) * (fSize))
#define _tanWave(fAngle, fFreq, fSize) (tanf((fAngle) * (fFreq)) * (fSize))
#define _triangleWave(fAngle, fFreq, fSize) ((fabsf(fmodf((fAngle) * (fFreq), 4.0f) - 2.0f) - 1.0f) * fSize)
#define _sawtoothWave(fAngle, fFreq, fSize) (fmodf((fAngle) * (fFreq), 1.0f) * ((fSize) * 2.0f - (fSize)))
#define _squareWave(fAngle, fFreq, fSize) (_sineWave(fAngle, fFreq, 1.0f) >= 0.0f ? fSize : -fSize)

VOID Xorshift32_Seed(DWORD dwSeed);
DWORD Xorshift32();
HFONT GetWingdingsFont(INT nSize);
BOOL IsCompatibleWindowsVersion();
BOOL DisableDWM();
BOOL Windows10LayeredWindowSupport(HWND hWnd);
BOOL GdiOverlayFallbackWindow();
VOID GetRandomChars(WCHAR* pszBuffer, SIZE_T dwLength);
VOID ReadRegistry();
BOOL Dropper();