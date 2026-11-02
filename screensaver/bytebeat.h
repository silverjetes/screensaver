#pragma once
#include "screensaver.h"

typedef struct _BYTEBEAT
{
	CHAR* pchBuffer;
	DWORD dwSampleRate;
} BYTEBEAT;

typedef struct _BYTEBEAT_SAMPLE
{
	CHAR chLeft;
	CHAR chRight;
} BYTEBEAT_SAMPLE;

typedef BYTEBEAT_SAMPLE(*BYTEBEAT_CALLBACK)(INT t);

#define BYTEBEAT_LENGTH 30

extern BYTEBEAT bytebeats[15];

BOOL PlayBytebeat(BYTEBEAT beat);
VOID InitializeBytebeats();
BYTEBEAT MakeBytebeatStruct(BYTEBEAT_CALLBACK fnCallback, DWORD dwSampleRate);
DWORD WINAPI BytebeatThread(LPVOID lv);

BYTEBEAT_SAMPLE Bytebeat1(INT t);
BYTEBEAT_SAMPLE Bytebeat2(INT t);
BYTEBEAT_SAMPLE Bytebeat3(INT t);
BYTEBEAT_SAMPLE Bytebeat4(INT t);
BYTEBEAT_SAMPLE Bytebeat5(INT t);
BYTEBEAT_SAMPLE Bytebeat6(INT t);
BYTEBEAT_SAMPLE Bytebeat7(INT t);
BYTEBEAT_SAMPLE Bytebeat8(INT t);
BYTEBEAT_SAMPLE Bytebeat9(INT t);
BYTEBEAT_SAMPLE Bytebeat10(INT t);
BYTEBEAT_SAMPLE Bytebeat11(INT t);
BYTEBEAT_SAMPLE Bytebeat12(INT t);
BYTEBEAT_SAMPLE Bytebeat13(INT t);
BYTEBEAT_SAMPLE Bytebeat14(INT t);
BYTEBEAT_SAMPLE Bytebeat15(INT t);