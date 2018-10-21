#pragma once

#include "Math/PointAndSize.h"
#include "InputKeys.h"
#include "Core.h"

class SGE_EXPORT Input
{
public:
	static Point GetMausePosition();
	static bool IsKeyDown(int scancode);	
	static bool IsMouseKeyDown(int scancode);
};