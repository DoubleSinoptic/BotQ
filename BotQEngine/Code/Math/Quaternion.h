#pragma once

#include "Vector3.h"
#include "Math.h"
#include <float.h>


#define RAD_TO_DEG (180.0f / Mathf::Pi())
#define DEG_TO_RAD (Mathf::Pi() / 180.0f)

inline float NormalizeAngle(float c) 
{
	float p2 = Mathf::Pi() * 2.0f;
	if(c > 0.0)
		for (; c > p2; c -= p2) {}
	else
		for (; c < -p2; c += p2) {}
	return c;
}
inline Vector3 NormalizeAngle(const Vector3& c)
{
	return { NormalizeAngle(c.x), NormalizeAngle(c.y), NormalizeAngle(c.z) };
}



class Matrix3
{
public:
	float m[3][3];

	inline Matrix3() {}

	inline Matrix3(float diag)
	{
		m[0][0] = diag;
		m[0][1] = 0.0f;
		m[0][2] = 0.0f;

		m[1][0] = 0.0f;
		m[1][1] = diag;
		m[1][2] = 0.0f;

		m[2][0] = 0.0f;
		m[2][1] = 0.0f;
		m[2][2] = diag;
	}

	inline Matrix3(const Vector3& a, const Vector3& b, const Vector3& c)
	{
		m[0][0] = a.x;
		m[0][1] = b.x;
		m[0][2] = c.x;

		m[1][0] = a.y;
		m[1][1] = b.y;
		m[1][2] = c.y;

		m[2][0] = a.z;
		m[2][1] = b.z;
		m[2][2] = c.z;
	}

	inline const float* operator[] (size_t row) const
	{
		return (float*)m[row];
	}

	inline float* operator[] (size_t row)
	{
		return (float*)m[row];
	}

	bool ToEulerAngles(float& xAngle, float& yAngle, float& zAngle) const
	{
		xAngle = -asinf(m[1][2]);
		if (xAngle < Mathf::HalfPi())
		{
			if (xAngle > -Mathf::HalfPi())
			{
				yAngle = atan2f(m[0][2], m[2][2]);
				zAngle = atan2f(m[1][0], m[1][1]);

				return true;
			}
			else
			{
				xAngle = -Mathf::HalfPi();
				yAngle = atan2f(-m[0][1], m[0][0]);
				zAngle = (0.0f);

				return false;
			}
		}
		else
		{
			xAngle = Mathf::HalfPi();
			yAngle = atan2f(m[0][1], m[0][0]);
			zAngle = (0.0f);

			return false;
		}
	}
};

inline Vector3 operator*(const Matrix3& a, const Vector3& vec)
{
	Vector3 prod;
	for (size_t row = 0; row < 3; row++)
	{
		prod[row] =
			a[row][0] * vec[0] +
			a[row][1] * vec[1] +
			a[row][2] * vec[2];
	}

	return prod;
}


class Quaternion 
{
public:
	FORCEINLINE void ToRotationMatrix(Matrix3& mat) const
	{
		float tx = x + x;
		float ty = y + y;
		float fTz = z + z;
		float twx = tx * w;
		float twy = ty * w;
		float twz = fTz * w;
		float txx = tx * x;
		float txy = ty * x;
		float txz = fTz * x;
		float tyy = ty * y;
		float tyz = fTz * y;
		float tzz = fTz * z;

		mat[0][0] = 1.0f - (tyy + tzz);
		mat[0][1] = txy - twz;
		mat[0][2] = txz + twy;
		mat[1][0] = txy + twz;
		mat[1][1] = 1.0f - (txx + tzz);
		mat[1][2] = tyz - twx;
		mat[2][0] = txz - twy;
		mat[2][1] = tyz + twx;
		mat[2][2] = 1.0f - (txx + tyy);
	}

	FORCEINLINE Quaternion() {}

	FORCEINLINE Quaternion(const Quaternion& f)
		: x(f.x), y(f.y), z(f.z), w(f.w)
	{}

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

	FORCEINLINE explicit Quaternion(const Vector3& eulerAngles)
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

	FORCEINLINE void SetEuler(const float& xa, const float& ya, const float& za)
	{
		Vector3 s, c;
		Mathf::SinCos(0.5f * Vector3(xa, ya, za) * DEG_TO_RAD, s, c);

		x = s.x * c.y * c.z + s.y * s.z * c.x;
		y = s.y * c.x * c.z - s.x * s.z * c.y;
		z = s.z * c.x * c.y - s.x * s.y * c.z;
		w = c.x * c.y * c.z + s.y * s.z * s.x;

	}

	FORCEINLINE Vector3 GetEuler() const
	{
		Matrix3 c;
		ToRotationMatrix(c);
		Vector3 eul;
		c.ToEulerAngles(eul.x, eul.y, eul.z);
		return eul * RAD_TO_DEG;
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
		float fNorm = w * w + x * x + y * y + z * z;
		if (fNorm > 0.0f)
		{
			float fInvNorm = 1.0f / fNorm;
			return Quaternion(-x * fInvNorm, -y * fInvNorm, -z * fInvNorm, w*fInvNorm);
		}
		else
		{
			// Return an invalid result to flag the error
			return Quaternion(0.0, 0.0, 0.0, 0.0);
		}
		/*return Quaternion(-floats[0], -floats[1], -floats[2], floats[3]);*/
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

	static FORCEINLINE Quaternion Slerp(const Quaternion& q, const Quaternion& q1, const float& t) 
	{	
		return q.Slerp(q1, t);		
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
	Matrix3 c;
	q.ToRotationMatrix(c);
	return c * posf;
	//Vector3 u(q.x, q.y, q.z);
	//float s = q.w;
	//return 2.0f * u.Dot(posf) * u
	//	+ (s*s - u.Dot(u)) * posf
	//	+ 2.0f * s * u.Cross(posf);
}
