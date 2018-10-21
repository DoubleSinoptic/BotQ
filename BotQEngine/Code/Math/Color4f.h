#pragma once

struct Color4f
{
	float r, g, b, a;
	Color4f(float _r, float _g, float _b, float _a)
		: r(_r), g(_g), b(_b), a(_a)
	{}
};

inline Color4f operator *(const Color4f& a, const Color4f& b)
{
	return Color4f(a.r * b.r,
		a.g * b.g,
		a.b * b.b,
		a.a * b.a);
}

inline Color4f operator +(const Color4f& a, const Color4f& b)
{
	return Color4f(a.r + b.r,
		a.g + b.g,
		a.b + b.b,
		a.a + b.a);
}