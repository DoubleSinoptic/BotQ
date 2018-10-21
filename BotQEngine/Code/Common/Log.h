#pragma once

#include "Defines.h"
#include "Exception.h"


template<typename... Args>
constexpr void Error(const char* fmt, const Args&... args)
{
	throw Exception(fmt, args...);
}
	
void SGE_EXPORT Log(const char* format, ...);

void SGE_EXPORT LogRewrite(const char* format, ...);

void SGE_EXPORT Warrning(const char* format, ...);