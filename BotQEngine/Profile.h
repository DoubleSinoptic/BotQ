#pragma once

#include "Core.h"
#include "Time.hpp"

class SGE_EXPORT Profile
{
public:
	Profile() = delete;
	static void PushRezult(const char* const string, TimeSpan time);
	static String ComputeRezult();
};
#define ENABLE_PROFILE
#ifdef ENABLE_PROFILE
#define PROFILE(x,str) \
{\
	TimeSpan curent = Time::GetTotalMicroseconds();\
	x;\
	TimeSpan rez = Time::GetTotalMicroseconds() - curent;\
	Profile::PushRezult(str, rez);\
}
#else
#define PROFILE(x,str) x
#endif
