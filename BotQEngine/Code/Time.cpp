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

TimeSpan frameStep = 16666;
TimeSpan step = 16666;
int		 maxFramesPerUpdate = 32;
double Time::GetDeltaTime()
{
	return GameInstance::GetCurrent()->delta;
}

double Time::GetRenderDeltaTime()
{
	return GameInstance::GetCurrent()->renderDelta;
}

TimeSpan Time::GetRenderStepSpan() 
{
	return frameStep;
}

TimeSpan Time::GetStepSpan()
{
	return step;
}

double Time::GetRenderStep()
{
	return frameStep * 0.000001;
}

double Time::GetStep()
{
	return step * 0.000001;;
}

void Time::SetRenderStep(double t)
{
	frameStep = TimeSpan(t / 1000000.0);
}

void Time::SetStep(double t) 
{
	step = TimeSpan(t / 1000000.0);
}

int Time::GetMaxUpdatesPerFrame()
{
	return maxFramesPerUpdate;
}

void Time::SetMaxUpdatesPerFrame(int s)
{
	maxFramesPerUpdate = s;
}