#include "AudioInstance.h"
#include <al.h>
#include <alc.h>

struct AudioInstanceImpl
{
	ALCdevice *device;
	ALCcontext *context;
public:
	AudioInstanceImpl() :
		device(nullptr),
		context(nullptr)
	{	
		device = alcOpenDevice(NULL);
		if (!device)
			throw Exception("error auido: %d", alGetError());
		context = alcCreateContext(device, NULL);
		if (!context)
			throw Exception("error auido: %d", alGetError());
	}
	~AudioInstanceImpl()
	{	
		if (context)
			alcDestroyContext(context);
		if (device)
			alcCloseDevice(device);	
	}
};

AudioInstance::AudioInstance() :
	mImpl(new AudioInstanceImpl())
{}

AudioInstance::~AudioInstance()
{
	if (mImpl)
		delete mImpl;
}

void AudioInstance::MakeCurrent()
{
	if (!alcMakeContextCurrent(mImpl->context))
		throw Exception("error auido: %d", alGetError());
}
