#pragma once

#include "Vector4.h"

typedef unsigned char UnsignedColorChanel;

struct Color
{
	UnsignedColorChanel r, g, b, a;
	FORCEINLINE Color() {}
	Color(UnsignedColorChanel _r, UnsignedColorChanel _g, UnsignedColorChanel _b, UnsignedColorChanel _a)
		: r(_r), g(_g), b(_b), a(_a)
	{}

	FORCEINLINE Vector4 ToVector4() const
	{
		return Vector4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	}
};

FORCEINLINE Color operator *(const Color& a, const Color& b)
{
	return Color(
		UnsignedColorChanel(((float(a.r) / 255.0f) * (float(b.r) / 255.0f)) * 255.0f),
		UnsignedColorChanel(((float(a.g) / 255.0f) * (float(b.g) / 255.0f)) * 255.0f),
		UnsignedColorChanel(((float(a.b) / 255.0f) * (float(b.b) / 255.0f)) * 255.0f),
		UnsignedColorChanel(((float(a.a) / 255.0f) * (float(b.a) / 255.0f)) * 255.0f)
	);
}

FORCEINLINE Color operator +(const Color& a, const Color& b)
{
	return Color(a.r + b.r,
		a.g + b.g,
		a.b + b.b, 
		a.a + b.a);
}

FORCEINLINE bool operator ==(const Color& a, const Color& b)
{
	return a.a == b.a && a.r == b.r && a.b == b.b && a.g == b.g;
}