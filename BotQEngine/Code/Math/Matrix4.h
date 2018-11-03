#pragma once

#include "Vector4.h"
#include "Quaternion.h"

class Matrix4 
{
	
public:
	Vector4 M[4];
	FORCEINLINE Matrix4() {}

	FORCEINLINE Matrix4(float diagValue)
	{
		M[0][0] = diagValue; M[0][1] = 0.0f; M[0][2] = 0.0f; M[0][3] = 0.0f;
		M[1][0] = 0.0f; M[1][1] = diagValue; M[1][2] = 0.0f; M[1][3] = 0.0f;
		M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = diagValue; M[2][3] = 0.0f;
		M[3][0] = 0.0f; M[3][1] = 0.0f; M[3][2] = 0.0f; M[3][3] = diagValue;
	}

	FORCEINLINE void FillZero() 
	{
		M[0][0] = 0.0f; M[0][1] = 0.0f; M[0][2] = 0.0f; M[0][3] = 0.0f;
		M[1][0] = 0.0f; M[1][1] = 0.0f; M[1][2] = 0.0f; M[1][3] = 0.0f;
		M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 0.0f; M[2][3] = 0.0f;
		M[3][0] = 0.0f; M[3][1] = 0.0f; M[3][2] = 0.0f; M[3][3] = 0.0f;		
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
		Matrix4 m = Matrix4::Identity();
		m[0][0] = v.x; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
		m[1][0] = 0.0f; m[1][1] = v.y; m[1][2] = 0.0f; m[1][3] = 0.0f;
		m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = v.z; m[2][3] = 0.0f;
		m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
		return m;
	}
	
	static Matrix4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
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
  /*  static Matrix4 LookAtRetro(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        Matrix4 Matrix;
        Vector3 X, Y, Z;
        Z = eye - center;
        Z.Normalize();
        Y = up;
        X = Y.Cross(Z);
        X.Normalize();
        Y.Normalize();
        Matrix[0][0] = X.x;
        Matrix[1][0] = X.y;
        Matrix[2][0] = X.z;
        Matrix[3][0] = -X.Dot(eye);
        Matrix[0][1] = Y.x;
        Matrix[1][1] = Y.y;
        Matrix[2][1] = Y.z;
        Matrix[3][1] = -Y.Dot(eye);
        Matrix[0][2] = Z.x;
        Matrix[1][2] = Z.y;
        Matrix[2][2] = Z.z;
        Matrix[3][2] = -Z.Dot(eye);
        Matrix[0][3] = 0;
        Matrix[1][3] = 0;
        Matrix[2][3] = 0;
        Matrix[3][3] = 1.0f;
        return Matrix;
    }*/

	static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
	{

		/*Vector3 f = (eye - center).Normalize();
		Vector3 s = (f.Cross(up)).Normalize();
		Vector3 u = s.Cross(f);

		Matrix4 Result(1.0f);

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
		return Result;*/

		Matrix4 Matrix;
		Vector3 X, Y, Z;
		Z = eye - center;
		Z.Normalize();
		Y = up;
		X = Y.Cross(Z);
		Y = Z.Cross(X);
		X.Normalize();
		Y.Normalize();
		Matrix[0][0] = X.x;
		Matrix[1][0] = X.y;
		Matrix[2][0] = X.z;
		Matrix[3][0] = -X.Dot(eye);
		Matrix[0][1] = Y.x;
		Matrix[1][1] = Y.y;
		Matrix[2][1] = Y.z;
		Matrix[3][1] = -Y.Dot(eye);
		Matrix[0][2] = Z.x;
		Matrix[1][2] = Z.y;
		Matrix[2][2] = Z.z;
		Matrix[3][2] = -Z.Dot(eye);
		Matrix[0][3] = 0;
		Matrix[1][3] = 0;
		Matrix[2][3] = 0;
		Matrix[3][3] = 1.0f;
		return Matrix;
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

	static Matrix4 PerspectiveRH(float fovy, float aspect, float near, float far)
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

	static Matrix4 Perspective(float y_fov, float aspect, float n, float f)
	{
		Matrix4 mret;
		Vector4* m = mret.M;
		float const a = 1.f / (float)tan(y_fov / 2.f);

		m[0][0] = a / aspect;
		m[0][1] = 0.f;
		m[0][2] = 0.f;
		m[0][3] = 0.f;

		m[1][0] = 0.f;
		m[1][1] = a;
		m[1][2] = 0.f;
		m[1][3] = 0.f;

		m[2][0] = 0.f;
		m[2][1] = 0.f;
		m[2][2] = -((f + n) / (f - n));
		m[2][3] = -1.f;

		m[3][0] = 0.f;
		m[3][1] = 0.f;
		m[3][2] = -((2.f * f * n) / (f - n));
		m[3][3] = 0.f;

		return mret;
	}

	/*FORCEINLINE Matrix4 operator*(const Matrix4& rhs) const
	{
		Matrix4 r;
#define m M

		r.m[0][0] = m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0] + m[0][2] * rhs.m[2][0] + m[0][3] * rhs.m[3][0];
		r.m[0][1] = m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1] + m[0][2] * rhs.m[2][1] + m[0][3] * rhs.m[3][1];
		r.m[0][2] = m[0][0] * rhs.m[0][2] + m[0][1] * rhs.m[1][2] + m[0][2] * rhs.m[2][2] + m[0][3] * rhs.m[3][2];
		r.m[0][3] = m[0][0] * rhs.m[0][3] + m[0][1] * rhs.m[1][3] + m[0][2] * rhs.m[2][3] + m[0][3] * rhs.m[3][3];

		r.m[1][0] = m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0] + m[1][2] * rhs.m[2][0] + m[1][3] * rhs.m[3][0];
		r.m[1][1] = m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1] + m[1][2] * rhs.m[2][1] + m[1][3] * rhs.m[3][1];
		r.m[1][2] = m[1][0] * rhs.m[0][2] + m[1][1] * rhs.m[1][2] + m[1][2] * rhs.m[2][2] + m[1][3] * rhs.m[3][2];
		r.m[1][3] = m[1][0] * rhs.m[0][3] + m[1][1] * rhs.m[1][3] + m[1][2] * rhs.m[2][3] + m[1][3] * rhs.m[3][3];

		r.m[2][0] = m[2][0] * rhs.m[0][0] + m[2][1] * rhs.m[1][0] + m[2][2] * rhs.m[2][0] + m[2][3] * rhs.m[3][0];
		r.m[2][1] = m[2][0] * rhs.m[0][1] + m[2][1] * rhs.m[1][1] + m[2][2] * rhs.m[2][1] + m[2][3] * rhs.m[3][1];
		r.m[2][2] = m[2][0] * rhs.m[0][2] + m[2][1] * rhs.m[1][2] + m[2][2] * rhs.m[2][2] + m[2][3] * rhs.m[3][2];
		r.m[2][3] = m[2][0] * rhs.m[0][3] + m[2][1] * rhs.m[1][3] + m[2][2] * rhs.m[2][3] + m[2][3] * rhs.m[3][3];

		r.m[3][0] = m[3][0] * rhs.m[0][0] + m[3][1] * rhs.m[1][0] + m[3][2] * rhs.m[2][0] + m[3][3] * rhs.m[3][0];
		r.m[3][1] = m[3][0] * rhs.m[0][1] + m[3][1] * rhs.m[1][1] + m[3][2] * rhs.m[2][1] + m[3][3] * rhs.m[3][1];
		r.m[3][2] = m[3][0] * rhs.m[0][2] + m[3][1] * rhs.m[1][2] + m[3][2] * rhs.m[2][2] + m[3][3] * rhs.m[3][2];
		r.m[3][3] = m[3][0] * rhs.m[0][3] + m[3][1] * rhs.m[1][3] + m[3][2] * rhs.m[2][3] + m[3][3] * rhs.m[3][3];
#undef m
		return r;
	}*/
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
