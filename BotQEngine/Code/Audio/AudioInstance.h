#pragma once

#include "../Core.h"

class SGE_EXPORT AudioInstance
{
	class AudioInstanceImpl* mImpl;
public:
	AudioInstance();
	~AudioInstance();
	void MakeCurrent();
};