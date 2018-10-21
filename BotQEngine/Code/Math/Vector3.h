#pragma once

#include "../Common/Defines.h"
#include <math.h>

class Vector3 
{
public:
	FORCEINLINE Vector3() {}

	FORCEINLINE Vector3(float _x, float _y, float _z)
	: x(_x), y(_y) , z(_z) {}

	FORCEINLINE Vector3(const Vector3& v)
		: x(v.x), y(v.y), z(v.z) {}

	FORCEINLINE Vector3& operator =(const Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	FORCEINLINE Vector3& operator+=(const Vector3& v)
	{
		floats[0] += v.floats[0];
		floats[1] += v.floats[1];
		floats[2] += v.floats[2];
		return *this;
	}


	FORCEINLINE Vector3& operator-=(const Vector3& v)
	{
		floats[0] -= v.floats[0];
		floats[1] -= v.floats[1];
		floats[2] -= v.floats[2];
		return *this;
	}

	FORCEINLINE Vector3& operator*=(const float& s)
	{
		floats[0] *= s;
		floats[1] *= s;
		floats[2] *= s;
		return *this;
	}

	FORCEINLINE Vector3& operator/=(const float& s)
	{
		return *this *= float(1.0) / s;
 	}

	FORCEINLINE float Dot(const Vector3& v) const
	{

		return floats[0] * v.floats[0] +
			floats[1] * v.floats[1] +
			floats[2] * v.floats[2];
	}


	FORCEINLINE float LengthSquared() const
	{
		return Dot(*this);
	}

	FORCEINLINE float Length() const
	{
		return sqrtf(LengthSquared());
	}

	FORCEINLINE float& operator [](int index) 
	{
		return floats[index];
	}

	FORCEINLINE float operator [](int index) const
	{
		return floats[index];
	}

	FORCEINLINE Vector3 Normalized()
	{
		Vector3 e = *this;
		return e /= Length();
	}

	FORCEINLINE Vector3& Normalize()
	{
		return *this /= Length();
	}

	FORCEINLINE Vector3 Cross(const Vector3& v) const
	{
		return Vector3(
			floats[1] * v.floats[2] - floats[2] * v.floats[1],
			floats[2] * v.floats[0] - floats[0] * v.floats[2],
			floats[0] * v.floats[1] - floats[1] * v.floats[0]);
	}

	FORCEINLINE Vector3 Lerp(const Vector3& v, const float& t) const
	{
		return
			Vector3(floats[0] + (v.floats[0] - floats[0]) * t,
				floats[1] + (v.floats[1] - floats[1]) * t,
				floats[2] + (v.floats[2] - floats[2]) * t);
	}

	FORCEINLINE Vector3& operator*=(const Vector3& v)
	{
		floats[0] *= v.floats[0];
		floats[1] *= v.floats[1];
		floats[2] *= v.floats[2];
		return *this;
	}

	FORCEINLINE	bool operator==(const Vector3& other) const
	{	
		return ((floats[2] == other.floats[2]) &&
			(floats[1] == other.floats[1]) &&
			(floats[0] == other.floats[0]));
	}

	FORCEINLINE	bool	operator!=(const Vector3& other) const
	{
		return !(*this == other);
	}

	static Vector3 Identity() { return Vector3(0.0f, 0.0f, 0.0f); };

	static Vector3 Zero() { return Vector3(0.0f, 0.0f, 0.0f); };

	static Vector3 Infinity() { return Vector3(1.0e35, 1.0e35, 1.0e35); };


	FORCEINLINE void Floor(const Vector3& cmp)
	{
		if (cmp.x < x) x = cmp.x;
		if (cmp.y < y) y = cmp.y;
		if (cmp.z < z) z = cmp.z;
	}

	
	FORCEINLINE void Ceil(const Vector3& cmp)
	{
		if (cmp.x > x) x = cmp.x;
		if (cmp.y > y) y = cmp.y;
		if (cmp.z > z) z = cmp.z;
	}

	union 
	{
		struct 
		{
			float x, y, z;
		};
		struct
		{
			float floats[3];
		};
	};	
};


//basic operations
FORCEINLINE Vector3 operator+(const Vector3& v1, const Vector3& v2)
{

	return Vector3(
		v1.floats[0] + v2.floats[0],
		v1.floats[1] + v2.floats[1],
		v1.floats[2] + v2.floats[2]);

}

FORCEINLINE Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
	return Vector3(
		v1.floats[0] * v2.floats[0],
		v1.floats[1] * v2.floats[1],
		v1.floats[2] * v2.floats[2]);

}

FORCEINLINE Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	return Vector3(
		v1.floats[0] - v2.floats[0],
		v1.floats[1] - v2.floats[1],
		v1.floats[2] - v2.floats[2]);
}

FORCEINLINE Vector3 operator/(const Vector3& v1, const Vector3& v2)
{
	return Vector3(
		v1.floats[0] / v2.floats[0],
		v1.floats[1] / v2.floats[1],
		v1.floats[2] / v2.floats[2]);
}

FORCEINLINE Vector3 operator-(const Vector3& floats)
{
	return Vector3(-floats[0], -floats[1], -floats[2]);
}

//once float operations
FORCEINLINE Vector3 operator*(const Vector3& v, const float& s)
{
	return Vector3(v.floats[0] * s, v.floats[1] * s, v.floats[2] * s);
}

FORCEINLINE Vector3 operator*(const float& s, const Vector3& v)
{
	return v * s;
}

FORCEINLINE Vector3 operator/(const Vector3& v, const float& s)
{
	return v * (float(1.0) / s);
}

