#pragma once

#include "Math.h"
#include "Vector3.h"
#include <float.h>

class BBox3 
{
	Vector3 min;
	Vector3 max;
public:
	BBox3() 
	{
		Reset();
	}

	BBox3(const Vector3& postion, const Vector3& size)
	{
		Reset();
		Vector3 sz = size / 2.0f;
		ExtendPoint(postion + Vector3(sz));
		ExtendPoint(postion - Vector3(sz));
	}

	Vector3 GetCenter() const 
	{
		return (min + max) / 2;
	}

	Vector3 GetSize() const 
	{
		Vector3 rt = min - max;
		return Vector3(Mathf::Abs(rt.x), Mathf::Abs(rt.y), Mathf::Abs(rt.z));
	}

	void Reset() 
	{
		min = Vector3(1e+8f, 1e+8f, 1e+8f);
		max = Vector3(-1e+8f, -1e+8f, -1e+8f);
	}

	void ExtendPoint(const Vector3& vec) 
	{
		if (vec.x > max.x)
			max.x = vec.x;
		if (vec.y > max.y)
			max.y = vec.y;
		if (vec.z > max.z)
			max.z = vec.z;

		if (vec.x < min.x)
			min.x = vec.x;
		if (vec.y < min.y)
			min.y = vec.y;
		if (vec.z < min.z)
			min.z = vec.z;
	}

};