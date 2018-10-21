#pragma once

#include "StorageFunction.h"

class Guard 
{
	StorageFunction<void(void)> mFunc;
public:
	template<typename T>
	Guard(const T& fnc)
		: mFunc(fnc)
	{}
	~Guard() 
	{
		if (mFunc)
			mFunc();
	}
};