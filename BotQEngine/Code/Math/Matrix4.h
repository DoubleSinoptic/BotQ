#pragma once

#include "Vector4.h"
#include "Quaternion.h"

class Matrix4 
{
	
public:
	Vector4 M[4];
	FORCEINLINE Matrix4() {}

	FORCEINLINE Matrix4(float x)
	{
		M[0] = Vector4(x, 0.0, 0.0, 0.0);
		M[1] = Vector4(0.0, x, 0.0, 0.0);
		M[2] = Vector4(0.0, 0.0, x, 0.0);
		M[3] = Vector4(0.0, 0.0, 0.0, x);
	}

	FORCEINLINE void FillZero() 
	{
		M[0] = Vector4(0., 0.0, 0.0, 0.0);
		M[1] = Vector4(0.0, 0., 0.0, 0.0);
		M[2] = Vector4(0.0, 0.0, 0., 0.0);
		M[3] = Vector4(0.0, 0.0, 0.0, 0.);
	}

	Matrix4 Transpose() const 
	{
		Matrix4 r;
		r.SetRow(0, this->operator [](0));
		r.SetRow(1, this->operator [](1));
		r.SetRow(2, this->operator [](2));
		r.SetRow(3, this->operator [](3));
		return r;
	}

	static Matrix4 FromQuaternion(const Quaternion& q)
	{
		Matrix4 m = Matrix4::Identity();
		double sqw = q.w*q.w;
		double sqx = q.x*q.x;
		double sqy = q.y*q.y;
		double sqz = q.z*q.z;

		double invs = 1 / (sqx + sqy + sqz + sqw);
		m[0][0] = (sqx - sqy - sqz + sqw)*invs; 
		m[1][1] = (-sqx + sqy - sqz + sqw)*invs;
		m[2][2] = (-sqx - sqy + sqz + sqw)*invs;

		double tmp1 = q.x*q.y;
		double tmp2 = q.z*q.w;
		m[1][0] = 2.0 * (tmp1 + tmp2)*invs;
		m[0][1] = 2.0 * (tmp1 - tmp2)*invs;

		tmp1 = q.x*q.z;
		tmp2 = q.y*q.w;
		m[2][0] = 2.0 * (tmp1 - tmp2)*invs;
		m[0][2] = 2.0 * (tmp1 + tmp2)*invs;
		tmp1 = q.y*q.z;
		tmp2 = q.x*q.w;
		m[2][1] = 2.0 * (tmp1 + tmp2)*invs;
		m[1][2] = 2.0 * (tmp1 - tmp2)*invs;
		return m;
	}

	static Matrix4 Identity() 
	{
		Matrix4 mret;
		Vector4* m = mret.M;
		m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		return mret;
	}

	static Matrix4 Zero() 
	{
		Matrix4 mret;
		Vector4* m = mret.M;
		m[0][0] = 0.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = 0.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 0.0f; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 0.0f;
		return mret;
	}

	static Matrix4 Scale(const Vector3& v)
	{
		Matrix4 m;
		m.M[0] = Vector4(v.x, 0.0, 0.0, 0.0);
		m.M[1] = Vector4(0.0, v.y, 0.0, 0.0);
		m.M[2] = Vector4(0.0, 0.0, v.z, 0.0);
		m.M[3] = Vector4(0.0, 0.0, 0.0, 1.0);
		return m;
	}
	static const bool leftHanded = true;
	static Matrix4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		if (leftHanded)
		{
			Matrix4 Result(1);
			Result[0][0] = static_cast<float>(2) / (right - left);
			Result[1][1] = static_cast<float>(2) / (top - bottom);
			Result[2][2] = static_cast<float>(2) / (zFar - zNear);
			Result[3][0] = -(right + left) / (right - left);
			Result[3][1] = -(top + bottom) / (top - bottom);
			Result[3][2] = -(zFar + zNear) / (zFar - zNear);
			return Result;
		}
		else 
		{
			Matrix4 Result(1);
			Result[0][0] = static_cast<float>(2) / (right - left);
			Result[1][1] = static_cast<float>(2) / (top - bottom);
			Result[3][0] = -(right + left) / (right - left);
			Result[3][1] = -(top + bottom) / (top - bottom);
			Result[2][2] = -static_cast<float>(2) / (zFar - zNear);
			Result[3][2] = -(zFar + zNear) / (zFar - zNear);

			return Result;
		}
	
	}



	static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
	{
		if (leftHanded)
		{
			Vector3 const f = ((center - eye).Normalized());
			Vector3 const s = ((up.Cross(f)).Normalized());
			Vector3 const u = f.Cross(s);

			Matrix4 Result(1.0);
			Result[0][0] = s.x;
			Result[1][0] = s.y;
			Result[2][0] = s.z;
			Result[0][1] = u.x;
			Result[1][1] = u.y;
			Result[2][1] = u.z;
			Result[0][2] = f.x;
			Result[1][2] = f.y;
			Result[2][2] = f.z;
			Result[3][0] = -s.Dot(eye);
			Result[3][1] = -u.Dot(eye);
			Result[3][2] = -f.Dot(eye);
			return Result;
		}
		else 
		{
			Vector3 const f = ((center - eye).Normalized());
			Vector3 const s = ((f.Cross(up)).Normalized());
			Vector3 const u = s.Cross(f);

			Matrix4 Result(1.0);
			Result[0][0] = s.x;
			Result[1][0] = s.y;
			Result[2][0] = s.z;
			Result[0][1] = u.x;
			Result[1][1] = u.y;
			Result[2][1] = u.z;
			Result[0][2] = -f.x;
			Result[1][2] = -f.y;
			Result[2][2] = -f.z;
			Result[3][0] = -s.Dot(eye);
			Result[3][1] = -u.Dot(eye);
			Result[3][2] = f.Dot(eye);
			return Result;
		}
	}

	Vector3 GetTranslationComponent() const
	{
		return Vector3((*this)[3][0], (*this)[3][1], (*this)[3][2]);
	}

	void SetPositionComponent(const Vector3& vec) 
	{
		(*this)[3][0] = vec.x;
		(*this)[3][1] = vec.y;
		(*this)[3][2] = vec.z;
	}

	const float* Data() const
	{
		return &(M[0].x);
	}

	float* Data() 
	{
		return &(M[0].x);
	}

	FORCEINLINE const Vector4& operator [](int index) const
	{
		return M[index];
	}

	FORCEINLINE Vector4& operator [](int index)
	{
		return M[index];
	}

	FORCEINLINE Vector4 Row(int id) const
	{
		return Vector4(M[0][id], M[1][id], M[2][id], M[3][id]);
	}

	FORCEINLINE void SetRow(int id, const Vector4& c) 
	{
		M[0][id] = c.x;
		M[1][id] = c.y;
		M[2][id] = c.z;
		M[3][id] = c.w;
	}

	static Matrix4 Perspective(float fovy, float aspect, float near, float far)
	{
		if (leftHanded)
		{
			float const tanHalfFovy = tan(fovy / static_cast<float>(2));

			Matrix4 Result(0.0);
			Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
			Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
			Result[2][2] = (far + near) / (far - near);
			Result[2][3] = static_cast<float>(1);
			Result[3][2] = -(static_cast<float>(2) * far * near) / (far - near);
			return Result;
		}
		else 
		{
			float const tanHalfFovy = tan(fovy / static_cast<float>(2));

			Matrix4 Result(0.0);
			Result[0][0] = static_cast<float>(1) / (aspect * tanHalfFovy);
			Result[1][1] = static_cast<float>(1) / (tanHalfFovy);
			Result[2][2] = -(far + near) / (far - near);
			Result[2][3] = -static_cast<float>(1);
			Result[3][2] = -(static_cast<float>(2) * far * near) / (far - near);

			return Result;
		}
	}

};
//
FORCEINLINE Matrix4 operator*(const Matrix4& m1, const Matrix4& m2)
{
	Vector4 const SrcA0 = m1[0];
	Vector4 const SrcA1 = m1[1];
	Vector4 const SrcA2 = m1[2];
	Vector4 const SrcA3 = m1[3];

	Vector4 const SrcB0 = m2[0];
	Vector4 const SrcB1 = m2[1];
	Vector4 const SrcB2 = m2[2];
	Vector4 const SrcB3 = m2[3];

	Matrix4 Result;
	Result[0] = SrcA0 * SrcB0[0] + SrcA1 * SrcB0[1] + SrcA2 * SrcB0[2] + SrcA3 * SrcB0[3];
	Result[1] = SrcA0 * SrcB1[0] + SrcA1 * SrcB1[1] + SrcA2 * SrcB1[2] + SrcA3 * SrcB1[3];
	Result[2] = SrcA0 * SrcB2[0] + SrcA1 * SrcB2[1] + SrcA2 * SrcB2[2] + SrcA3 * SrcB2[3];
	Result[3] = SrcA0 * SrcB3[0] + SrcA1 * SrcB3[1] + SrcA2 * SrcB3[2] + SrcA3 * SrcB3[3];
	return Result;

}

FORCEINLINE Vector4 operator*(const Matrix4& m, const Vector4& v)
{
	Vector4 rz;
	rz.x = m[0].Dot(v);
	rz.y = m[1].Dot(v);
	rz.z = m[2].Dot(v);
	rz.w = m[3].Dot(v);
	return rz;
}
