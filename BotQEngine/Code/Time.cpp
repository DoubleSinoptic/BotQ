#include "GameInstance.h"
#include "Time.hpp"

#include <chrono>

std::chrono::high_resolution_clock::time_point clockX = std::chrono::high_resolution_clock::now();

void Time::ResetStart()
{
	clockX = std::chrono::high_resolution_clock::now();
}

double Time::GetTotalTime()
{
	std::chrono::high_resolution_clock::duration delta = std::chrono::high_resolution_clock::now() - clockX;
	return double(std::chrono::duration_cast<std::chrono::microseconds>(delta).count()) * 0.000001;
}

TimeSpan Time::GetTotalMicroseconds()
{
	std::chrono::high_resolution_clock::duration delta = std::chrono::high_resolution_clock::now() - clockX;
	return std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
}

double Time::GetDeltaTime()
{
	return GameInstance::GetCurrent()->delta;
}

double Time::GetRenderDeltaTime()
{
	return GameInstance::GetCurrent()->renderDelta;
}
