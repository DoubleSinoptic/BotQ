#include "Profile.h"

struct ProfileRezult 
{
	const char* name;
	TimeSpan time;
};

DynamicArray<ProfileRezult> rezults;

void Profile::PushRezult(const char* const string, TimeSpan time)
{
	rezults.Add({string, time});
}

String Profile::ComputeRezult() 
{
	String s = "[Timings]";



	return s;
}