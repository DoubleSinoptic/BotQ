#pragma once
#include "../Component.h"
#include "../GameObject.h"

class SGE_EXPORT AudioClip
{
	uint32_t buffer;
public:
	explicit AudioClip(const DynamicArray<char>& rawWav);
	~AudioClip();
	uint32_t GetAlHandle() const;
};


class SGE_EXPORT AudioSource : public Component
{
	EventHandler<> onTransformChanged;
	uint32_t sourceId;
	Ref<AudioClip> clip;
public:
	static void SetupListener(const Vector3& p, const Quaternion& r);

	void Destroy() override;
	void Awake() override;
	void Play();
	
	void SetClip(const Ref<AudioClip>& clip);

	REGISTER_PROPETRY(bool, Looping)
	void SetLooping(bool val);
	bool GetLooping() const;
};