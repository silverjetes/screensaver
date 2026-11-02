#include "screensaver.h"

static DWORD Xorshift32_State = 1;

VOID Xorshift32_Seed(DWORD dwSeed)
{
	Xorshift32_State = max(dwSeed, 1);
}

DWORD Xorshift32()
{
	DWORD x = Xorshift32_State;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	Xorshift32_State = x;
	return x;
}

HFONT GetWingdingsFont(INT nSize)
{
	return CreateFontW(nSize, 0, 0, 0,
		FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Wingdings");
}

BOOL IsCompatibleWindowsVersion()
{
	BOOL bSupported = FALSE;

	HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
	if (!hNtdll) return FALSE;

	RtlGetVersion_pdef RtlGetVersion = (RtlGetVersion_pdef)GetProcAddress(hNtdll, "RtlGetVersion");
	if (!RtlGetVersion) goto cleanup;

	RTL_OSVERSIONINFOW os = { 0 };
	os.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

	RtlGetVersion(&os);

	if (os.dwMajorVersion == 5) /* Windows XP */
	{
		bSupported = TRUE;
		goto cleanup;
	}

	if (os.dwMajorVersion == 6 && os.dwMinorVersion <= 1) /* Windows Vista and Windows 7 */
	{
		g_bIsVistaOr7 = TRUE;
		bSupported = TRUE;
		goto cleanup;
	}

	if (os.dwMajorVersion >= 10 && os.dwBuildNumber >= 19041) /* Windows 10 2004+ */
	{
		g_bIs102004OrNewer = TRUE;
		bSupported = TRUE;
		goto cleanup;
	}

	goto cleanup;

cleanup:
	FreeLibrary(hNtdll);
	return bSupported;
}

BOOL DisableDWM()
{
	BOOL bSuccess = FALSE;

	HMODULE hDwmapi = LoadLibraryW(L"dwmapi.dll");
	if (!hDwmapi) return FALSE;

	DwmEnableComposition_pdef DwmEnableComposition = (DwmEnableComposition_pdef)GetProcAddress(hDwmapi, "DwmEnableComposition");
	if (!DwmEnableComposition) goto cleanup;

	bSuccess = (DwmEnableComposition(0) == S_OK);
	goto cleanup;

cleanup:
	FreeLibrary(hDwmapi);
	return bSuccess;
}

BOOL Windows10LayeredWindowSupport(HWND hWnd)
{
	BOOL bSuccess = FALSE;

	HMODULE hUser32 = LoadLibraryW(L"user32.dll");
	if (!hUser32) return FALSE;

	SetWindowDisplayAffinity_pdef SetWindowDisplayAffinity = (SetWindowDisplayAffinity_pdef)GetProcAddress(hUser32, "SetWindowDisplayAffinity");
	if (!SetWindowDisplayAffinity) goto cleanup;

	bSuccess = SetWindowDisplayAffinity(hWnd, 0x00000011 /* WDA_EXCLUDEFROMCAPTURE */);
	goto cleanup;

cleanup:
	FreeLibrary(hUser32);
	return bSuccess;
}

BOOL GdiOverlayFallbackWindow()
{
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"ScreensaverShaderWnd";
	wc.lpszMenuName = NULL;
	wc.lpfnWndProc = DefWindowProcW;
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hInstance = g_hInstance;
	wc.hIcon = NULL;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassW(&wc)) return FALSE;

	g_hWnd = CreateWindowExW(
		0, L"ScreensaverShaderWnd", L"screensaver.exe", 0,
		0, 0, 0, 0, NULL, NULL, g_hInstance, NULL);

	return (g_hWnd != NULL);
}

VOID GetRandomChars(WCHAR* pszBuffer, SIZE_T dwLength)
{
	for (DWORD i = 0; i < dwLength; i++)
		pszBuffer[i] = (WCHAR)(Xorshift32() % 0x3FFF + 0x3FFF);
	pszBuffer[dwLength - 1] = L'\0';
}

VOID ReadRegistry()
{
	HKEY hKey;

	if (RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\screensaver", 0, NULL, 0, KEY_READ, NULL, &hKey, NULL))
		return;

	DWORD dwType = REG_DWORD;
	DWORD dwValue = 0;
	DWORD dwSize = sizeof(DWORD);

	if (!RegQueryValueExW(hKey, L"Block", NULL, &dwType, (BYTE*)&dwValue, &dwSize))
		g_bBlocked = (dwType == REG_DWORD && dwValue >= 1);

	if (!RegQueryValueExW(hKey, L"SafeMode", NULL, &dwType, (BYTE*)&dwValue, &dwSize))
		g_bSafeMode = (dwType == REG_DWORD && dwValue >= 1);

	if (!RegQueryValueExW(hKey, L"DisableLayeredWindows", NULL, &dwType, (BYTE*)&dwValue, &dwSize))
		g_bLayeredGdiCompatible = !(dwType == REG_DWORD && dwValue >= 1);

	if (!RegQueryValueExW(hKey, L"Seed", NULL, &dwType, (BYTE*)&dwValue, &dwSize) &&
		(dwType == REG_DWORD))
		Xorshift32_Seed(dwValue);

	RegCloseKey(hKey);
}

BOOL Dropper()
{
	HKEY hKey;

	if (RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\screensaver", 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hKey, NULL))
		return TRUE;

	DWORD dwType;
	WCHAR szData[MAX_PATH] = { 0 };
	DWORD cbData = MAX_PATH;

	if (RegQueryValueExW(hKey, L"Path", NULL, &dwType, (LPBYTE)&szData, &cbData))
		return TRUE;

	if (dwType != REG_SZ || !szData)
		return TRUE;

	return DeleteFileW(szData);
}