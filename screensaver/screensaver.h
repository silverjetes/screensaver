#pragma once

#include <stdio.h>
#include <math.h>

#pragma warning (push, 0)
#include <Windows.h>
#include <Shlwapi.h>
#include <psapi.h>
#pragma warning (pop)

#include "gdi.h"
#include "colors.h"
#include "bytebeat.h"
#include "payloads.h"
#include "functions.h"
#include "destruction.h"
#include "resource.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Msimg32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "kernel32.lib")

extern HINSTANCE g_hInstance;

extern BOOL g_bIsVistaOr7;
extern BOOL g_bIs102004OrNewer;

extern BOOL g_bBlocked;
extern BOOL g_bSafeMode;
extern BOOL g_bLayeredGdiCompatible;