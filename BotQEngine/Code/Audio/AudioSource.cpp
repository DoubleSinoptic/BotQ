#include "AudioSource.h"
#include <al.h>

void AudioSource::SetupListener(const Vector3 & p, const Quaternion & r)
{
	Vector3 f[2];
	f[0] = r * Vector3(1.0, 0.0, 0.0);
	f[1] = r * Vector3(0.0, 1.0, 0.0);
	alListener3f(AL_POSITION, p.x, p.y, p.z);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, (float*)f);
}

void AudioSource::Destroy()
{
	GetGameObject()->GetHandlers()->OnTransformChanged -= &onTransformChanged;
	alDeleteSources(1, &sourceId);
}

void AudioSource::Awake()
{
	alGenSources(1, &sourceId);
	alSource3f(sourceId, AL_VELOCITY, 0, 0, 0);
	alSourcef(sourceId, AL_PITCH, 1);
	alSourcef(sourceId, AL_GAIN, 1);
	onTransformChanged = EventHandler<>([&]() 
	{
		Vector3 v = GetGameObject()->GetPosition();
		alSource3f(sourceId, AL_POSITION, v.x, v.y, v.z);
	}, false);
	GetGameObject()->GetHandlers()->OnTransformChanged += &onTransformChanged;
}

void AudioSource::Play()
{
	alSourcePlay(sourceId);
}

void AudioSource::SetClip(const Ref<AudioClip>& clip)
{
	this->clip = clip;
	alSourcei(sourceId, AL_BUFFER, clip->GetAlHandle());
}

void AudioSource::SetLooping(bool val) 
{
	alSourcei(sourceId, AL_LOOPING, val ? AL_TRUE : AL_FALSE);
}

bool AudioSource::GetLooping() const 
{
	ALint c;
	alGetSourcei(sourceId, AL_LOOPING, &c);
	return c == AL_TRUE;
}


struct wavfile_header {
	char	riff_tag[4];
	int	    riff_length;
	char	wave_tag[4];
	char	fmt_tag[4];
	int	    fmt_length;
	short	audio_format;
	short	num_channels;
	int	    sample_rate;
	int	    byte_rate;
	short	block_align;
	short	bits_per_sample;
	char	data_tag[4];
	int	    data_length;
};

ALenum ToAlFormat(short channels, short samples)
{
	bool stereo = (channels > 1);

	switch (samples) {
	case 16:
		if (stereo)
			return AL_FORMAT_STEREO16;
		else
			return AL_FORMAT_MONO16;
	case 8:
		if (stereo)
			return AL_FORMAT_STEREO8;
		else
			return AL_FORMAT_MONO8;
	default:
		return -1;
	}
}

AudioClip::AudioClip(const DynamicArray<char>& rawWav) 
{
	alGenBuffers(1, &buffer);

	const char* beg = rawWav.GetData();
	const wavfile_header* hdr = reinterpret_cast<const wavfile_header*>(beg);
	const char* PCMbeg = beg + sizeof(wavfile_header);

	alBufferData(buffer, ToAlFormat(hdr->num_channels, hdr->bits_per_sample),
		PCMbeg, hdr->data_length, hdr->sample_rate);
}

AudioClip::~AudioClip()
{
	alDeleteBuffers(1, &buffer);
}

uint32_t AudioClip::GetAlHandle() const
{
	return buffer;
}
