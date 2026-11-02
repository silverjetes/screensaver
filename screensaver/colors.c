#include "screensaver.h"

HSL RGBtoHSL(RGB rgb)
{
	HSL hsl = { 0 };

	FLOAT r = min(max(rgb.r / 255.0f, 0.0f), 1.0f);
	FLOAT g = min(max(rgb.g / 255.0f, 0.0f), 1.0f);
	FLOAT b = min(max(rgb.b / 255.0f, 0.0f), 1.0f);

	FLOAT min = min(min(r, g), b);
	FLOAT max = max(max(r, g), b);
	FLOAT delta = max - min;

	hsl.l = (max + min) / 2.0f;

	if (delta == 0.0f)
	{
		hsl.h = 0.0f;
		hsl.s = 0.0f;
		return hsl;
	}

	hsl.s = hsl.l < 0.5f ? delta / (max + min) : delta / (2.0f - max - min);

	if (max == r)
		hsl.h = 60.0f * fmodf((g - b) / delta, 6.0f);
	else if (max == g)
		hsl.h = 60.0f * (((b - r) / delta) + 2.0f);
	else if (max == b)
		hsl.h = 60.0f * (((r - g) / delta) + 4.0f);

	if (hsl.h < 0.0f) hsl.h += 360.0f;

	return hsl;
}

RGB HSLtoRGB(HSL hsl)
{
	RGB rgb = { 0 };

	FLOAT h = fmodf(hsl.h, 360.0f);
	if (h < 0.0f)
		h += 360.0f;
	FLOAT s = min(max(hsl.s, 0.0f), 1.0f);
	FLOAT l = min(max(hsl.l, 0.0f), 1.0f);

	FLOAT q = l < 0.5 ? l * (1.0f + s) : l + s - l * s;
	FLOAT p = 2.0f * l - q;
	FLOAT h_k = h / 360.0f;

	rgb.r = (BYTE)(HueToRGB(p, q, h_k + 1.0f / 3.0f) * 255.0f);
	rgb.g = (BYTE)(HueToRGB(p, q, h_k) * 255.0f);
	rgb.b = (BYTE)(HueToRGB(p, q, h_k - 1.0f / 3.0f) * 255.0f);

	return rgb;
}

FLOAT HueToRGB(FLOAT p, FLOAT q, FLOAT t)
{
	if (t < 0.0f) t += 1.0f;
	if (t > 1.0f) t -= 1.0f;

	if (t < 1.0f / 6.0f)
		return p + (q - p) * 6.0f * t;
	if (t < 1.0f / 2.0f)
		return q;
	if (t < 2.0f / 3.0f)
		return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;

	return p;
}