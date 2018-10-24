#pragma once
#include <cmath>
class Mathf
{
public:

	static inline float Log(float a, float b)
	{
		return  std::log(a) / std::log(b);
	}

	static inline bool Equal(float a, float b, float epsilon = 0.000001)
	{
		if (Mathf::Abs(a - b) > epsilon)
			return false;
		return true;
	}

	static float Abs(float value)
	{
		return (value >= 0.0f) ? value : -value;
	}
	
	static float Sign(float value)
	{
		return (value >= 0.0f) ? 1.0f : -1.0f;	
	}

	static float Pi()
	{
		return 3.14159265359f;
	}

	static float Max(float v, float v1)
	{
		if (v > v1)
			return v;
		return v1;
	}

	static float Min(float v, float v1)
	{
		if (v < v1)
			return v;
		return v1;
	}

	
	static float Clamp(float v, float min, float max)
	{
		if (v < min)
			return min;
		else if (v > max)
			return max;
		return v;	
	}

	static float Clamp01(float val)
	{
		return Clamp(val, 0.0f, 1.0f);
	}

	static float Store(float value)
	{
		float vf = value * Abs(value);
		float qu = value * Abs(Sign(value) * 2.5f);
		float de = value * Abs(Sign(value / -1.0f) * Sign(value * value * value));
		return (vf + qu + de) / 9.0f;
	}
};
