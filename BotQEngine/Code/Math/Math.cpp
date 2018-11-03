#include "Math.h"
#include "Vector3.h"

Vector3 Mathf::Sin(const Vector3& a)
{
	return Vector3(sinf(a.x), sinf(a.y), sinf(a.z));
}

Vector3 Mathf::Cos(const Vector3& a)
{
	return Vector3(cosf(a.x), cosf(a.y), cosf(a.z));
}

void Mathf::SinCos(const Vector3& a, Vector3& s, Vector3& c)
{
	s = Sin(a);
	c = Cos(a);
}
