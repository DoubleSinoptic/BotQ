#pragma once

#include "Component.h"
#include "Graphics/Texture.h"
#include "Math/BBox3.h"

struct ReflectionProbeState 
{
	BBox3		m_bbox;
	float		m_captureRadius;
	Texture*	m_defuse;
	Texture*	m_specular;
};

class ReflectionProbe : public Component
{
	ReflectionProbeState m_state;
public:
	void SetBBox3(const BBox3& bbox);
	void SetDefuse(Texture* texture);
	void SetSpecular(Texture* texture);
	void SetRadius(float radius);
	const ReflectionProbeState& GetState() const;
	void Awake() override;
	void Destroy() override;
	void Capture(const StorageFunction<void(void)>& f);
	ReflectionProbe();
	~ReflectionProbe();
};

