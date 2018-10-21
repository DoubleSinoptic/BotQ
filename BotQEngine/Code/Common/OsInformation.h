#pragma once

#include "Defines.h"

class SGE_EXPORT SystemInfo
{
public:
	static const char* OsVersionString();
	static int BitsForWord();
	static bool IsLitleEndian();
};
