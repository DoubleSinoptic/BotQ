#pragma once

#include "../Core.h"

class AudioInstance
{
	class AudioInstanceImpl* mImpl;
public:
	AudioInstance();
	~AudioInstance();
	void MakeCurrent();
};