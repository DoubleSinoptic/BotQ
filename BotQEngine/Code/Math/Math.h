#pragma once
#include <cmath>
#include "Vector3.h"
class SGE_EXPORT Mathf
{
public:
	template<class T>
	static constexpr T DivideAndRoundUp(T n, T d)
	{
		return (n + d - 1) / d; 
	}

	static float Log(float a, float b)
	{
		return  std::log(a) / std::log(b);
	}

	static float Random();
	static Vector3 Sin(const Vector3& a);
	static Vector3 Cos(const Vector3& a);
	static void SinCos(const Vector3& a, Vector3& s, Vector3& c);

	template<class T, class A, class B>
	static constexpr T Lerp(T a, A b, B f)
	{
		return a + T(f) * (T(b) - T(a));
	}

	static constexpr bool Equal(float a, float b, float epsilon = 0.000001)
	{
		if (Mathf::Abs(a - b) > epsilon)
			return false;
		return true;
	}

	template<class T>
	static constexpr T Abs(T value)
	{
		return (value >= 0.0f) ? value : -value;
	}

	template<class T>
	static constexpr T Sign(T value)
	{
		return (value >= 0.0f) ? 1.0f : -1.0f;	
	}

	static constexpr float Pi()
	{
		return 3.14159265359f;
	}
	static constexpr float HalfPi()
	{
		return 3.14159265359f / 2.0f;
	}

	template<class T, class X>
	static constexpr float Max(T v, X v1)
	{
		if (v > v1)
			return v;
		return v1;
	}

	template<class T, class X>
	static constexpr float Min(T v, X v1)
	{
		if (v < v1)
			return v;
		return v1;
	}

	template<class T, class X, class B>
	static constexpr T Clamp(T v, X min, B max)
	{
		if (v < min)
			return min;
		else if (v > max)
			return max;
		return v;	
	}

	static constexpr float Clamp01(float val)
	{
		return Clamp(val, 0.0f, 1.0f);
	}

	static constexpr float FloatRestore(float value)
	{
		float v1 = value * Clamp01(value * 0.1);
		float v2 = value * Clamp01(v1 * 0.01);
		float v3 = value * Clamp01(v2 * 0.001);
		return (v1 + v2 + v3) / 9.0f;
	}
};
