#pragma once

#include "Common/Defines.h"
#include <stdint.h>

typedef   uint64_t TimeSpan;

inline TimeSpan TimeSpamFromSeconds(double secodns)
{
	return TimeSpan(secodns * 1000000.0);
}

inline TimeSpan TimeSpamFromSeconds(float secodns)
{
	return TimeSpan(double(secodns) * 1000000.0);
}

inline TimeSpan TimeSpamFromSeconds(int secodns)
{
	return TimeSpan(secodns) * 1000000;
}

inline float TimeSpawnToFloatSeconds(const TimeSpan& span)
{
	return float(span) * 0.000001;
}

inline int TimeSpawnToIntSeconds(const TimeSpan& span)
{
	return int(span) / 1000000;
}


class SGE_EXPORT Time
{
public:
	static void ResetStart();
  	static double GetTotalTime();
	static TimeSpan GetTotalMicroseconds();
	static double GetDeltaTime();
	static double GetRenderDeltaTime();
};