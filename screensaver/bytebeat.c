#include "screensaver.h"

BYTEBEAT bytebeats[15] = { 0 };

BOOL PlayBytebeat(BYTEBEAT bb)
{
	if (bb.pchBuffer == NULL)
		return FALSE;

	HWAVEOUT hWaveOut;

	WAVEFORMATEX wfx = { 0 };
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = 2;
	wfx.wBitsPerSample = 8;
	wfx.nSamplesPerSec = bb.dwSampleRate;
	wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	if (waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL)) return FALSE;

	WAVEHDR wh = { 0 };
	wh.lpData = bb.pchBuffer;
	wh.dwBufferLength = wfx.nSamplesPerSec * BYTEBEAT_LENGTH * wfx.nChannels;
	wh.dwFlags = 0;
	wh.dwLoops = 0;

	if (waveOutPrepareHeader(hWaveOut, &wh, sizeof(wh))) return FALSE;
	if (waveOutWrite(hWaveOut, &wh, sizeof(wh))) return FALSE;

	Sleep(BYTEBEAT_LENGTH * 1000);

	waveOutReset(hWaveOut);
	waveOutUnprepareHeader(hWaveOut, &wh, sizeof(wh));
	waveOutClose(hWaveOut);

	return TRUE;
}

VOID InitializeBytebeats()
{
	for (INT i = 0; i < _countof(bytebeats); i++)
	{
		if (bytebeats[i].pchBuffer != NULL) return;
	}

	bytebeats[0] = MakeBytebeatStruct(Bytebeat1, 8000);
	bytebeats[1] = MakeBytebeatStruct(Bytebeat2, 48000);
	bytebeats[2] = MakeBytebeatStruct(Bytebeat3, 16000);
	bytebeats[3] = MakeBytebeatStruct(Bytebeat4, 48000);
	bytebeats[4] = MakeBytebeatStruct(Bytebeat5, 22050);
	bytebeats[5] = MakeBytebeatStruct(Bytebeat6, 8000);
	bytebeats[6] = MakeBytebeatStruct(Bytebeat7, 8000);
	bytebeats[7] = MakeBytebeatStruct(Bytebeat8, 16000);
	bytebeats[8] = MakeBytebeatStruct(Bytebeat9, 8000);
	bytebeats[9] = MakeBytebeatStruct(Bytebeat10, 48000);
	bytebeats[10] = MakeBytebeatStruct(Bytebeat11, 16000);
	bytebeats[11] = MakeBytebeatStruct(Bytebeat12, 16000);
	bytebeats[12] = MakeBytebeatStruct(Bytebeat13, 16000);
	bytebeats[13] = MakeBytebeatStruct(Bytebeat14, 8000);
	bytebeats[14] = MakeBytebeatStruct(Bytebeat15, 32000);
}

BYTEBEAT MakeBytebeatStruct(BYTEBEAT_CALLBACK fnCallback, DWORD dwSampleRate)
{
	DWORD dwSize = dwSampleRate * BYTEBEAT_LENGTH * 2;
	CHAR* pchBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);

	for (DWORD t = 0; t < dwSize / 2; t++)
	{
		BYTEBEAT_SAMPLE bs = fnCallback(t);
		pchBuffer[t * 2] = bs.chLeft;
		pchBuffer[t * 2 + 1] = bs.chRight;
	}

	return (BYTEBEAT)
	{
		pchBuffer, dwSampleRate
	};
}

DWORD WINAPI BytebeatThread(LPVOID lv)
{
	for (;;)
	{
		INT nIndex;
		do
		{
			nIndex = Xorshift32() % _countof(bytebeats);
		} while (bytebeats[nIndex].pchBuffer == NULL);

		PlayBytebeat(bytebeats[nIndex]);

		HeapFree(GetProcessHeap(), 0, bytebeats[nIndex].pchBuffer);
		bytebeats[nIndex].pchBuffer = NULL;
	} lv;
}

BYTEBEAT_SAMPLE Bytebeat1(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)(_sawtoothWave((FLOAT)t, (FLOAT)(t / 500), 1.0f)) | (t * t >> ((t / 70) % 9))),
		(CHAR)((INT)(_sawtoothWave((FLOAT)t, (FLOAT)(t / 490), 1.0f)) | (t * t >> ((t / 100) % 8)))
	};
}

BYTEBEAT_SAMPLE Bytebeat2(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((t >> 6 * t) | (t * t >> (t % 16))),
		(CHAR)((t >> 5 * t) | (t * t >> (t % 11)))
	};
}

BYTEBEAT_SAMPLE Bytebeat3(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)(sinf((FLOAT)t) * (FLOAT)(t >> 5 % 255)),
		(CHAR)(cosf((FLOAT)t) * (FLOAT)(t >> 5 % 255))
	};
}

BYTEBEAT_SAMPLE Bytebeat4(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)(sinf((FLOAT)t) * (FLOAT)(t >> 5 % 255)) | (INT)(cosf((FLOAT)t) * (FLOAT)(t >> 5 % 255))),
		(CHAR)(tan((FLOAT)t) * (FLOAT)(t >> 5 % 255))
	};
}

BYTEBEAT_SAMPLE Bytebeat5(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((t >> 6 * t) | (t * t >> (t % 16))),
		(CHAR)((t >> 5 * t) | (t * t >> (t % 20)))
	};
}

BYTEBEAT_SAMPLE Bytebeat6(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((t * t >> ((t / 50) % 16)) | ((t * t >> ((t / 40) % 12)) | 128) | (t / 8000 % 2 ? 230 : 0)),
		(CHAR)((t * t >> ((t / 47) % 18)) | ((t * t >> ((t / 60) % 19)) | 128) | (t / 8000 % 2 ? 0 : 230))
	};
}

BYTEBEAT_SAMPLE Bytebeat7(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)((FLOAT)t * sinf(cosf((FLOAT)t / 1000.0f))) & (INT)((FLOAT)t * cosf(sinf((FLOAT)t / 2500.0f))) ^ (((INT)((FLOAT)t * cosf(cosf((FLOAT)t / 2500.0f))) | 100))),
		(CHAR)((INT)((FLOAT)t * cosf(sinf((FLOAT)t / 1500.0f))) & (INT)((FLOAT)t * sinf(cosf((FLOAT)t / 2000.0f))) ^ (((INT)((FLOAT)t * sinf(sinf((FLOAT)t / 2500.0f))) | 100)))
	};
}

BYTEBEAT_SAMPLE Bytebeat8(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)(t * ((t >> (INT)((sinf((FLOAT)t / 1000.0f) + 1.0f) * 7.0f)) ^ (INT)(sinf((FLOAT)t / 10000.0f) * 255.0f))),
		(CHAR)(t * ((t >> (INT)((cosf((FLOAT)t / 800.0f) + 1.0f) * 9.0f)) ^ (INT)(cosf((FLOAT)t / 15000.0f) * 255.0f)))
	};
}

BYTEBEAT_SAMPLE Bytebeat9(INT t)
{
	FLOAT x = _triangleWave((FLOAT)t, 0.02f, 1.0f);
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)(t * (fmodf((FLOAT)t, fabsf(sinf(fmodf((FLOAT)t, 3.0f)))))) ^ ((INT)((x * (sinf((FLOAT)t / 820.0f) + 1.0f))) | (INT)(sinf((FLOAT)t / 100.0f) * (FLOAT)t) >> 6)),
		(CHAR)((INT)(t * (fmodf((FLOAT)t, fabsf(cosf(fmodf((FLOAT)t, 3.0f)))))) ^ ((INT)((x * (cosf((FLOAT)t / 820.0f) + 1.0f))) | (INT)(cosf((FLOAT)t / 100.0f) * (FLOAT)t) >> 6))
	};
}

BYTEBEAT_SAMPLE Bytebeat10(INT t)
{
	FLOAT x = (FLOAT)((31.0f - ((FLOAT)t / (48000.0f * 5.0f) + 0.01f)));

	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)((FLOAT)t * ((FLOAT)((t / 8) >> 5) * sinf((FLOAT)t / 7500.0f) + 5.0f)) ^ (INT)((FLOAT)t * fmodf((FLOAT)t, fabsf(sinf((fmodf((FLOAT)t, 2.0f)) / x))))),
		(CHAR)((INT)((FLOAT)t * ((FLOAT)((t / 8) >> 5) * cosf((FLOAT)t / 7500.0f) + 5.0f)) ^ (INT)((FLOAT)t * fmodf((FLOAT)t, fabsf(cosf((fmodf((FLOAT)t, 2.0f)) / x)))))
	};
}

BYTEBEAT_SAMPLE Bytebeat11(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)(tanf((FLOAT)t / (fabsf(sinf((FLOAT)t / 500.0f) * 100.0f) + 0.1f))) & (INT)_sineWave((FLOAT)t / 100.0f, cosf((FLOAT)t / 500.0f), 255.0f)),
		(CHAR)((INT)(tanf((FLOAT)t / (fabsf(cosf((FLOAT)t / 700.0f) * 200.0f) + 0.1f))) & (INT)_cosineWave((FLOAT)t / 100.0f, sinf((FLOAT)t / 500.0f), 255.0f))
	};
}

BYTEBEAT_SAMPLE Bytebeat12(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((FLOAT)t / fmodf((FLOAT)t, sinf(t / 100000.0f) * 100.0f)),
		(CHAR)((FLOAT)t / fmodf((FLOAT)t, cosf(t / 100000.0f) * 100.0f))
	};
}

BYTEBEAT_SAMPLE Bytebeat13(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)(t / ((t % (t / 400 + 1) % 33) + 1)),
		(CHAR)(t / ((t % (t / 390 + 1) % 36) + 1))
	};
}

BYTEBEAT_SAMPLE Bytebeat14(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((t * (t >> (INT)tanf(fmodf(sinf(t / 1000.0f), cosf(fmodf(t * 1000.0f, 16.0f))))) ^ (t / 100)) ^ ((INT)((sinf(t / 500.0f) * t) * 24.0f) % 128)),
		(CHAR)((t * (t >> (INT)tanf(fmodf(cosf(t / 1000.0f), sinf(fmodf(t * 1000.0f, 16.0f))))) ^ (t / 100)) ^ ((INT)((cosf(t / 500.0f) * t) * 24.0f) % 128))
	};
}

BYTEBEAT_SAMPLE Bytebeat15(INT t)
{
	return (BYTEBEAT_SAMPLE)
	{
		(CHAR)((INT)((FLOAT)t / fmodf((FLOAT)t, 42.0f)) ^ (INT)((FLOAT)(t % 256) / fabsf(sinf(t / 1000.0f)))),
		(CHAR)((INT)((FLOAT)t * fmodf((FLOAT)t, 0.023f)) ^ (INT)((FLOAT)(t % 256) / fabsf(cosf(t / 1000.0f))))
	};
}