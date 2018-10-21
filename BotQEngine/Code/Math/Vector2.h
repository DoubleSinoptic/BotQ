#pragma once

#include "../Common/Defines.h"
#include <math.h>

class Vector2
{
public:
	FORCEINLINE Vector2() {}
	FORCEINLINE Vector2(float _x, float _y)
		: x(_x), y(_y) {}
	float x, y;

	FORCEINLINE Vector2 operator-() const
	{
		return Vector2(-x, -y);
	}

	FORCEINLINE float Dot(const Vector2& v)
	{
		return x * v.x + y * v.y;
	}

	FORCEINLINE Vector2 Normalize() const
	{
		float l = Length();
		return Vector2(x / l, y / l);
	}

	FORCEINLINE float Length()  const
	{
		return sqrtf(x * x + y * y);
	}

	static FORCEINLINE Vector2 Zero() 
	{
		return Vector2(0, 0);
	}
};

FORCEINLINE Vector2 operator +(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}
FORCEINLINE Vector2 operator -(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}
FORCEINLINE Vector2 operator *(const Vector2& a, float v)
{
	return Vector2(a.x * v, a.y * v);
}
FORCEINLINE Vector2 operator /(const Vector2& a, float v)
{
	return Vector2(a.x /v, a.y /v);
}
FORCEINLINE Vector2 operator /(const Vector2& a, const Vector2& v)
{
	return Vector2(a.x / v.x, a.y / v.y);
}
FORCEINLINE Vector2 operator *(float v, const Vector2& a)
{
	return Vector2(a.x * v, a.y * v);
}
