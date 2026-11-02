#pragma once
#include "screensaver.h"

DWORD WINAPI WindowPayloadThread(LPVOID lv);
BOOL CALLBACK WindowPayloadProc(HWND hWnd, LPARAM lParam);
DWORD WINAPI MouseButtonSwappingThread(LPVOID lv);