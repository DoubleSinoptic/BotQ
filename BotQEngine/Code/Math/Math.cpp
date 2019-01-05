#include "Math.h"
#include "Vector3.h"

#include <ctime>
#include <random>

float Mathf::Random()
{
	static struct A {
		unsigned int seed;
		A() : seed((unsigned int)time(nullptr)) { srand(seed); }
	} randomer;
	return float(double(rand()) / double(RAND_MAX));
}

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
