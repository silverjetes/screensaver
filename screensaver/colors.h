#pragma once
#include "screensaver.h"

typedef struct _RGB
{
	BYTE r;
	BYTE g;
	BYTE b;
} RGB;

typedef struct _HSL
{
	FLOAT h;
	FLOAT s;
	FLOAT l;
} HSL;

HSL RGBtoHSL(RGB rgb);
RGB HSLtoRGB(HSL hsl);
FLOAT HueToRGB(FLOAT p, FLOAT q, FLOAT t);