#include "AudioSource.h"
#include <al.h>

void AudioSource::SetupListener(const Vector3 & p, const Quaternion & r)
{
	ALenum error;
	Vector3 f[2];
	f[0] = r * Vector3(0.0, 0.0, 1.0);
	f[1] = r * Vector3(0.0, 1.0, 0.0);
	alListener3f(AL_POSITION, p.x, p.y, p.z);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	alListenerfv(AL_ORIENTATION, (float*)f);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
}

void AudioSource::Destroy()
{
	GetGameObject()->GetHandlers()->OnTransformChanged -= &onTransformChanged;
	alDeleteSources(1, &sourceId);
}

void AudioSource::Awake()
{
	ALenum error;
	alGenSources(1, &sourceId);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);

	alSource3f(sourceId, AL_VELOCITY, 0, 0, 0);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	alSourcef(sourceId, AL_PITCH, 1);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	alSourcef(sourceId, AL_GAIN, 1);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	Vector3 v = GetGameObject()->GetPosition();
	alSource3f(sourceId, AL_POSITION, v.x, v.y, v.z);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	onTransformChanged = EventHandler<>([&]() 
	{
		Vector3 v = GetGameObject()->GetPosition();
		alSource3f(sourceId, AL_POSITION, v.x, v.y, v.z);
	}, false);
	GetGameObject()->GetHandlers()->OnTransformChanged += &onTransformChanged;
}

void AudioSource::Play()
{
	ALenum error;
	alSourcePlay(sourceId);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
}

void AudioSource::SetClip(const Ref<AudioClip>& clip)
{
	ALenum error;
	this->clip = clip;
	alSourcei(sourceId, AL_BUFFER, clip->GetAlHandle());
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
}


void AudioSource::SetVolume(float vol)
{
	ALenum error;
	alSourcef(sourceId, AL_GAIN, Mathf::Clamp01(vol));
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
}

float AudioSource::GetVolume() const
{
	float c;
	ALenum error;
	alGetSourcef(sourceId, AL_GAIN, &c);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
	return c;
}

Ref<AudioClip> AudioSource::GetClip() const
{
	return clip;
}

void AudioSource::SetLooping(bool val) 
{
	ALenum error;
	alSourcei(sourceId, AL_LOOPING, val ? AL_TRUE : AL_FALSE);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
}

bool AudioSource::GetLooping() const 
{
	ALenum error;
	ALint c;
	alGetSourcei(sourceId, AL_LOOPING, &c);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);
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
	ALenum error;

	alGenBuffers(1, &buffer);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);

	const char* beg = rawWav.GetData();
	const wavfile_header* hdr = reinterpret_cast<const wavfile_header*>(beg);
	const char* PCMbeg = beg + sizeof(wavfile_header);

	alBufferData(buffer, ToAlFormat(hdr->num_channels, hdr->bits_per_sample),
		PCMbeg, hdr->data_length, hdr->sample_rate);

	if ((error = alGetError()) != AL_NO_ERROR)
		throw Exception("error auido: %d", error);

}

AudioClip::~AudioClip()
{
	alDeleteBuffers(1, &buffer);
}

uint32_t AudioClip::GetAlHandle() const
{
	return buffer;
}
