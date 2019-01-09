#include "Material.h"
#include "RenderSource.h"

RenderSource* Material::QuerySource(Mesh* r)
{
	if (r == nullptr)
		return nullptr;

	for (size_t i = 0; i < m_renderSources.LengthReference; i++)
		if (m_renderSources[i]->mesh == r)
			return m_renderSources[i];
	
	RenderSource* source = new RenderSource(r);

	renderThreadQueue.QueueFunction([=]() 
	{
		source->instancedData = new Gl3dArray<Matrix4>(Gl3dArrayTarget::Array);
		m_renderSources.Add(source);
	});

	return source;
}

void Material::NotifyLeaveSource(Mesh* r)
{
	if (r == nullptr)
		return;

	for (size_t i = 0; i < m_renderSources.LengthReference; i++)
		if (m_renderSources[i]->mesh == r)
		{
			renderThreadQueue.QueueFunction([=]()
			{
				delete m_renderSources[i]->instancedData;
				delete m_renderSources[i];
				m_renderSources.Remove(m_renderSources[i]);
			});		
		}
}

const DynamicArray<RenderSource*>& Material::GetRenderSources() const
{
	return m_renderSources;
}

Material::Material()
{
	renderThreadQueue.QueueFunction([=]() 
	{
		renderMaterials.Add(this);
	});
}

Material::~Material()
{
	renderThreadQueue.QueueFunctionWait([=]()
	{
		renderMaterials.Remove(this);
	});
}
