#include <stdio.h>
#include <Windows.h>
#include "resource.h"

VOID GetRandomChars(WCHAR* pszBuffer, SIZE_T dwLength)
{
	for (DWORD i = 0; i < dwLength; i++)
		pszBuffer[i] = (WCHAR)(rand() % 0x3FFF + 0x3FFF);
	pszBuffer[dwLength - 1] = L'\0';
}

INT WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	srand((UINT)__rdtsc());

	HANDLE hFile = NULL;

	BOOL bWow64 = FALSE;
	IsWow64Process(GetCurrentProcess(), &bWow64);

	HRSRC hResource = FindResourceW(NULL, MAKEINTRESOURCEW(bWow64 ? _64_BIT : _32_BIT), RT_RCDATA);
	if (hResource == NULL) goto fail;

	HGLOBAL hLoadedRsrc = LoadResource(NULL, hResource);
	if (hLoadedRsrc == NULL) goto fail;

	VOID* pvData = LockResource(hLoadedRsrc);
	if (!pvData) goto fail;

	DWORD dwSize = SizeofResource(NULL, hResource);

	WCHAR szTempPath[MAX_PATH];
	GetTempPathW(MAX_PATH, szTempPath);

	WCHAR szName[24];
	WCHAR szPath[MAX_PATH];

	GetRandomChars(szName, _countof(szName));
	swprintf_s(szPath, MAX_PATH, L"%s%s.scr", szTempPath, szName);

	hFile = CreateFileW(szPath,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE) goto fail;

	DWORD dwBytesWritten;
	if (!WriteFile(hFile, pvData, dwSize, &dwBytesWritten, NULL)) goto fail;

	CloseHandle(hFile);

	HKEY hKey;
	if (!RegCreateKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\screensaver", 0, NULL, 0, KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, NULL))
	{
		WCHAR szCurrentPath[MAX_PATH];
		GetModuleFileNameW(NULL, szCurrentPath, MAX_PATH);

		RegSetValueExW(hKey, L"Path", 0, REG_SZ, (const BYTE*)szCurrentPath, (lstrlenW(szCurrentPath) + 1) * 2);

		RegFlushKey(hKey);
		RegCloseKey(hKey);
	}

	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };

	si.cb = sizeof(STARTUPINFOW);

	if (CreateProcessW(szPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		MessageBoxW(NULL, L"Failed to execute screensaver!\nIt's set as a system file and is located somewhere in %TEMP%, sorry for that :P", L"screensaver.scr - ERROR", MB_ICONERROR);
		return 0;
	}

	hInstance; hPrevInstance; lpCmdLine; nShowCmd;
	return 0;
fail:
	MessageBoxW(NULL, L"Failed to extract screensaver!", L"screensaver.scr - ERROR", MB_ICONERROR);
	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)) CloseHandle(hFile);
	return 0;
}