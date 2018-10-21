#pragma once

#include "Core.h"

typedef void*(__cdecl *LevelLoaderCallback)();

#define REGISTER_LEVEL(LevelType) \
extern "C" SGE_EXPORT void* __cdecl LevelLoaderMain() \
{ \
	return (void*) new LevelType (); \
}

class SGE_EXPORT LevelBase 
{
public:
	virtual ~LevelBase() {}

	virtual void LoadLevel() = 0;
	virtual void UnloadLevel() = 0;
	virtual void UpdateLevel() = 0;
	
	static LevelBase* GetCurrent();
	static void LoadLevel(LevelBase* level);

};