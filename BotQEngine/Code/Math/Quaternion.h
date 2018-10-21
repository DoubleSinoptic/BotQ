#pragma once

#include "Vector3.h"
#include <float.h>

class Quaternion 
{
public:
	FORCEINLINE Quaternion() {}

	FORCEINLINE Quaternion(const float& _x, const float& _y, const float& _z, const float& _w)
		: x(_x), y(_y), z(_z), w(_w)
	{}

	FORCEINLINE Quaternion(const Vector3& _axis, const float& _angle)
	{
		SetRotation(_axis, _angle);
	}
	
	FORCEINLINE Quaternion(const float& pitch, const float& yaw, const float& roll)
	{
		SetEuler(pitch, yaw, roll);
	}

	FORCEINLINE Quaternion(const Vector3& eulerAngles)
	{
		SetEuler(eulerAngles.x, eulerAngles.y, eulerAngles.z);
	}

	FORCEINLINE void SetRotation(const Vector3& axis, const float& _angle)
	{
		float d = axis.Length();
	
		float s = sinf(_angle * float(0.5)) / d;
		SetValue(axis.x * s, axis.y * s, axis.z * s,
			cosf(_angle * float(0.5)));
	}
	
	FORCEINLINE void SetValue(const float& _x, const float& _y, const float& _z, const float& _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	FORCEINLINE void SetEuler(const float& pitch, const float& yaw, const float& roll)
	{
		float halfYaw = float(yaw) * float(0.5);
		float halfPitch = float(pitch) * float(0.5);
		float halfRoll = float(roll) * float(0.5);
		float cosYaw = cosf(halfYaw);
		float sinYaw = sinf(halfYaw);
		float cosPitch = cosf(halfPitch);
		float sinPitch = sinf(halfPitch);
		float cosRoll = cosf(halfRoll);
		float sinRoll = sinf(halfRoll);
		SetValue(cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
			cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
			sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
			cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw);
	}

	FORCEINLINE static Quaternion LookRotation(Vector3 forward, Vector3 up)
	{
#define __MATH_ROOT
		forward.Normalize();
		Vector3 right = (up.Cross(forward)).Normalized();
		up = forward.Cross(right);
		float m00 = right.x;
		float m01 = right.y;
		float m02 = right.z;
		float m10 = up.x;
		float m11 = up.y;
		float m12 = up.z;
		float m20 = forward.x;
		float m21 = forward.y;
		float m22 = forward.z;


		float num8 = (m00 + m11) + m22;
		Quaternion quaternion = Quaternion();
		if (num8 > 0.0f)
		{
			float num = __MATH_ROOT sqrtf(num8 + 1.0f);
			quaternion.w = num * 0.5f;
			num = 0.5f / num;
			quaternion.x = (m12 - m21) * num;
			quaternion.y = (m20 - m02) * num;
			quaternion.z = (m01 - m10) * num;
			return quaternion;
		}
		if ((m00 >= m11) && (m00 >= m22))
		{
			float num7 = __MATH_ROOT sqrtf(((1.0f + m00) - m11) - m22);
			float num4 = 0.5f / num7;
			quaternion.x = 0.5f * num7;
			quaternion.y = (m01 + m10) * num4;
			quaternion.z = (m02 + m20) * num4;
			quaternion.w = (m12 - m21) * num4;
			return quaternion;
		}
		if (m11 > m22)
		{
			float num6 = __MATH_ROOT sqrtf(((1.0f + m11) - m00) - m22);
			float num3 = 0.5f / num6;
			quaternion.x = (m10 + m01) * num3;
			quaternion.y = 0.5f * num6;
			quaternion.z = (m21 + m12) * num3;
			quaternion.w = (m20 - m02) * num3;
			return quaternion;
		}
		float num5 = __MATH_ROOT sqrtf(((1.0f + m22) - m00) - m11);
		float num2 = 0.5f / num5;
		quaternion.x = (m20 + m02) * num2;
		quaternion.y = (m21 + m12) * num2;
		quaternion.z = 0.5f * num5;
		quaternion.w = (m01 - m10) * num2;
		return quaternion;
#undef __MATH_ROOT
	}

	FORCEINLINE void SetEulerZYX(const float& yaw, const float& pitch, const float& roll)
	{
		float halfYaw = float(yaw) * float(0.5);
		float halfPitch = float(pitch) * float(0.5);
		float halfRoll = float(roll) * float(0.5);
		float cosYaw = cosf(halfYaw);
		float sinYaw = sinf(halfYaw);
		float cosPitch = cosf(halfPitch);
		float sinPitch = sinf(halfPitch);
		float cosRoll = cosf(halfRoll);
		float sinRoll = sinf(halfRoll);
		SetValue(sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw, //x
			cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw, //y
			cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw, //z
			cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw); //formerly yzx
	}
	
	FORCEINLINE	Quaternion& operator+=(const Quaternion& q)
	{	
		floats[0] += q.floats[0];
		floats[1] += q.floats[1];
		floats[2] += q.floats[2];
		floats[3] += q.floats[3];
		return *this;
	}

	FORCEINLINE Quaternion& operator-=(const Quaternion& q)
	{
		floats[0] -= q.floats[0];
		floats[1] -= q.floats[1];
		floats[2] -= q.floats[2];
		floats[3] -= q.floats[3];
		return *this;
	}

	FORCEINLINE Quaternion& operator*=(const float& s)
	{
		floats[0] *= s;
		floats[1] *= s;
		floats[2] *= s;
		floats[3] *= s;
		return *this;
	}

	FORCEINLINE Quaternion& operator*=(const Quaternion& q)
	{
		SetValue(
			floats[3] * q.x + floats[0] * q.floats[3] + floats[1] * q.z - floats[2] * q.y,
			floats[3] * q.y + floats[1] * q.floats[3] + floats[2] * q.x - floats[0] * q.z,
			floats[3] * q.z + floats[2] * q.floats[3] + floats[0] * q.y - floats[1] * q.x,
			floats[3] * q.floats[3] - floats[0] * q.x - floats[1] * q.y - floats[2] * q.z);
		return *this;
	}

	FORCEINLINE float Dot(const Quaternion& q) const
	{   
		return  floats[0] * q.x +
			floats[1] * q.y +
			floats[2] * q.z +
			floats[3] * q.floats[3];
	}

	FORCEINLINE float Length2() const
	{
		return Dot(*this);
	}

	FORCEINLINE float Length() const
	{
		return sqrtf(Length2());
	}

	FORCEINLINE Quaternion& Normalize()
	{
		return *this /= Length();
	}


	FORCEINLINE Quaternion
		operator*(const float& s) const
	{
		return Quaternion(x * s, y * s, z * s, floats[3] * s);
	}

	FORCEINLINE Quaternion operator/(const float& s) const
	{
		return *this * (float(1.0) / s);
	}


	FORCEINLINE Quaternion& operator/=(const float& s)
	{
		return *this *= float(1.0) / s;
	}

	FORCEINLINE Quaternion Normalized() const
	{
		return *this / Length();
	}
	
	FORCEINLINE float Angle(const Quaternion& q) const
	{
		float s = sqrtf(Length2() * q.Length2());
		return acosf(Dot(q) / s);
	}

	FORCEINLINE float GetAngle() const
	{
		float s = float(2.) * acosf(floats[3]);
		return s;
	}

	FORCEINLINE Vector3 GetAxis() const
	{
		float s_squared = 1.f - floats[3] * floats[3];

		if (s_squared < float(10.) * FLT_EPSILON) //Check for divide by zero
			return Vector3(1.0, 0.0, 0.0);  // Arbitrary
		float s = 1.f / sqrtf(s_squared);
		return Vector3(floats[0] * s, floats[1] * s, floats[2] * s);
	}

	
	FORCEINLINE Quaternion Inverse() const
	{
		return Quaternion(-floats[0], -floats[1], -floats[2], floats[3]);
	}

	
	FORCEINLINE Quaternion
		operator+(const Quaternion& q2) const
	{
		const Quaternion& q1 = *this;
		return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.floats[3] + q2.floats[3]);
	}

	FORCEINLINE Quaternion
		operator-(const Quaternion& q2) const
	{
		const Quaternion& q1 = *this;
		return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.floats[3] - q2.floats[3]);
	}

	FORCEINLINE Quaternion Slerp(const Quaternion& q, const float& t) const
	{
		const float magnitude = sqrtf(Length() * q.Length2());
	
		const float product = Dot(q) / magnitude;
		const float absproduct = fabsf(product);

		if (absproduct < float(1.0 - FLT_EPSILON))
		{
			const float theta = acosf(absproduct);
			const float d = sinf(theta);
	
			const float sign = (product < 0) ? float(-1) : float(1);
			const float s0 = sinf((float(1.0) - t) * theta) / d;
			const float s1 = sinf(sign * t * theta) / d;

			return Quaternion(
				(floats[0] * s0 + q.x * s1),
				(floats[1] * s0 + q.y * s1),
				(floats[2] * s0 + q.z * s1),
				(floats[3] * s0 + q.w * s1));
		}
		else
		{
			return *this;
		}
	}

	FORCEINLINE static const Quaternion& Identity()
	{
		static const Quaternion identityQuat(float(0.), float(0.), float(0.), float(1.));
		return identityQuat;
	}

	union
	{
		struct
		{
			float x, y, z, w;
		};
		struct
		{
			float floats[4];
		};
	};
};

FORCEINLINE Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(
		q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
		q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
		q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
		q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);
}

FORCEINLINE Vector3 operator*(const Quaternion &q, const Vector3 &posf)
{
	Vector3 u(q.x, q.y, q.z);
	float s = q.w;
	return 2.0f * u.Dot(posf) * u
		+ (s*s - u.Dot(u)) * posf
		+ 2.0f * s * u.Cross(posf);
}