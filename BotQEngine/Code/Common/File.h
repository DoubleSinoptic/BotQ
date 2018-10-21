#pragma once

#include "DynamicArray.h"
#include "String.h"
class File 
{
public:
	static DynamicArray<char> ReadAllBytes(const String& fileName);
	static String ReadAllText(const String& fileName);
};