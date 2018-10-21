#pragma once

#include "Vector2.h"
#include "../Common/Defines.h"
#include <math.h>

class Point 
{
public:
	FORCEINLINE Point() {}
	FORCEINLINE Point(int _x, int _y) : x(_x), y(_y)
	{}
	int x, y;
	float Length() 
	{
		return sqrtf(float((x * x) + (y * y)));
	}
	FORCEINLINE Vector2 ToVector2() const 
	{
		return Vector2(x, y);
	}
};

FORCEINLINE Point operator +(const Point& a, const Point& b)
{
	return { a.x + b.x, a.y + b.y };
}

FORCEINLINE Point operator -(const Point& a, const Point& b)
{
	return { a.x - b.x, a.y - b.y };
}

FORCEINLINE Point operator *(const Point& a, const Point& b)
{
	return { a.x * b.x, a.y * b.y };
}

FORCEINLINE Point operator /(const Point& a, const Point& b)
{
	return { a.x / b.x, a.y / b.y };
}

FORCEINLINE Point operator -(const Point& a)
{
	return { -a.x, -a.y};
}

class Size 
{
public: 
	FORCEINLINE Size() {}
	FORCEINLINE Size(int w, int h)
		: width(w), height(h)
	{}
	int width;
	int height;
	FORCEINLINE Vector2 ToVector2() const
	{
		return Vector2(width, height);
	}
};

FORCEINLINE Size operator +(const Size& a, const Size& b)
{
	return { a.width + b.width, a.height + b.height };
}
FORCEINLINE Size operator -(const Size& a, const Size& b)
{
	return { a.width - b.width, a.height - b.height };
}


FORCEINLINE bool operator ==(const Size& s, const Size& b)
{
	return (s.width == b.width) && (s.height == b.height);
}

FORCEINLINE bool operator !=(const Size& s, const Size& b)
{
	return !(s == b);
}

