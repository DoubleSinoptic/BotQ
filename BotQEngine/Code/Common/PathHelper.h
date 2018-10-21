#pragma once

#include "String.h"

class SGE_EXPORT Path
{
public:
	static String FileExt(const String& p);
	static String FileName(const String& p);
	static String FileDir(const String& p);
	static String PathFix(const String& str);
};
