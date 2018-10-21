#pragma once

#include "Common/Defines.h"

class SGE_EXPORT Time
{
public:
	static void ResetStart();
  	static double GetTotalTime();
	static double GetDeltaTime();
	static double GetRenderDeltaTime();
};