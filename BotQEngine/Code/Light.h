#pragma once

#include "Component.h"

enum class LightType : int32_t
{
	Directional = 0,
	Point = 1,
	Spot = 2
};

struct LightState 
{
	Vector3		m_color;
	LightType	m_type;
	float		m_intency;
	float		m_fov;
};

class Light : public Component
{
	LightState m_state;
public:
	Light();
	~Light();

	const LightState& GetState() const;

	void Awake() override;
	void Destroy() override;

	void SetColor(const Vector3& color);
	void SetIntensy(float value);
	void SetType(LightType type);
	void SetFov(float fov);
};

