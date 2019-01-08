#pragma once

#include "Vector3.h"

class Vector4
{
public:
	FORCEINLINE Vector4() {}
	union 
	{
		struct { float x; float y; float z; float w; };
		float array[4];
	};
	
	FORCEINLINE Vector4(const Vector3& vec, float _w)
		: x(vec.x), y(vec.y), z(vec.z), w(_w)
	{}

	FORCEINLINE Vector4(float _x, float _y, float _z, float _w)
		: x(_x), y(_y), z(_z), w(_w)
	{}

	FORCEINLINE Vector4(float _v)
		: x(_v), y(_v), z(_v), w(_v)
	{}

	FORCEINLINE Vector3 ToVector3() const
	{
		return Vector3(x,y,z);
	}

	FORCEINLINE float operator [](int id) const
	{
		//It's bad cast! xyzw not array!
		return array[id];
	}

	FORCEINLINE float& operator [](int id)
	{//It's bad cast! xyzw not array!
		return array[id];
	}

	FORCEINLINE float Dot(const Vector4& v) const
	{
		return v.x * x + v.y * y + v.z * z + v.w * w;
	}
};

FORCEINLINE Vector4 operator +(const Vector4& v, const Vector4& v0)
{
	return Vector4(v.ToVector3() + v0.ToVector3(), v.w + v0.w);
}

FORCEINLINE Vector4 operator -(const Vector4& v, const Vector4& v0)
{
	return Vector4(v.ToVector3() - v0.ToVector3(), v.w - v0.w);
}

FORCEINLINE Vector4 operator *(const Vector4& v, float v0)
{
	return Vector4(v.ToVector3() * v0, v.w * v0);
}