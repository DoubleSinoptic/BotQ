#include "Light.h"
#include "GameInstance.h"

void Light::Awake()
{
	renderThreadQueue.QueueFunction([=]() 
	{
		renderLights.Add(this);
	});
}

void Light::Destroy() 
{
	renderThreadQueue.QueueFunctionWait([=]()
	{
		renderLights.Remove(this);
	});

}

Light::Light() :
	m_state({Vector3(1.0, 1.0, 1.0), LightType::Directional, 1.0f, 90.f})
{}


Light::~Light()
{}

void Light::SetColor(const Vector3& color) 
{
	m_state.m_color = color;
}

void Light::SetIntensy(float value) 
{
	m_state.m_intency = value;
}

void Light::SetType(LightType type) 
{
	m_state.m_type = type;
}

void Light::SetFov(float fov)
{
	m_state.m_fov = fov;
}

const LightState& Light::GetState() const
{
	return m_state;
}