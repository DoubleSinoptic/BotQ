#include "ReflectionProbe.h"
#include "GameInstance.h"

void ReflectionProbe::Awake()
{
	renderThreadQueue.QueueFunction([=]()
	{
		renderReflectionProbes.Add(this);
	});
}

void ReflectionProbe::Destroy()
{
	renderThreadQueue.QueueFunctionWait([=]()
	{
		renderReflectionProbes.Remove(this);
	});
}

void ReflectionProbe::Capture(const StorageFunction<void(void)>& f)
{
	renderThreadQueue.QueueFunctionWait([=]()
	{
		renderReflectionProbes.Remove(this);
	});
}

ReflectionProbe::ReflectionProbe() :
	m_state({ BBox3(), 1e+7f, nullptr, nullptr })
{}

ReflectionProbe::~ReflectionProbe()
{}

void ReflectionProbe::SetBBox3(const BBox3& bbox)
{
	m_state.m_bbox = bbox;
}
void ReflectionProbe::SetDefuse(Texture* texture)
{
	m_state.m_defuse = texture;
}
void ReflectionProbe::SetSpecular(Texture* texture)
{
	m_state.m_specular = texture;
}
void ReflectionProbe::SetRadius(float radius) 
{
	m_state.m_captureRadius = radius;
}
const ReflectionProbeState& ReflectionProbe::GetState() const 
{
	return m_state;
}