#include "Level.h"

LevelBase* vak = nullptr;

LevelBase* LevelBase::GetCurrent() 
{
	return vak;
}


void LevelBase::LoadLevel(LevelBase* level) 
{
	if (vak)
	{
		vak->UnloadLevel();
		delete vak;
	}
	vak = level;
	vak->LoadLevel();
}