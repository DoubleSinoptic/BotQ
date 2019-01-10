#include "Material.h"
#include "RenderSource.h"
#include "GameInstance.h"

RenderSource* Material::QuerySource(Mesh* r)
{
	if (r == nullptr)
		return nullptr;

	for (size_t i = 0; i < m_renderSources.LengthReference; i++)
		if (m_renderSources[i]->GetMesh() == r)
			return m_renderSources[i];
	
	RenderSource* source = new RenderSource(r);
	renderThreadQueue.QueueFunction([=]()
	{
		m_renderSources.Add(source);
	});
	return source;

}

void Material::NotifyLeaveSource(RenderSource* r)
{
	if (r == nullptr)
		return;
	bool isDeleted = false;
	renderThreadQueue.QueueFunctionWait([&]()
	{			
		if (!r)
			return;
		if (!r->IsEmpty())
			return;
		isDeleted = true;
		m_renderSources.Remove(r);
	});			
	if(isDeleted)
		delete r;
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
		Log("material created core render: %p", this);
	});
	Log("material created core part: %p", this);
}

Material::~Material()
{
	for (size_t i = 0; i < m_renderSources.Length(); i++)
		delete m_renderSources[i];
	renderThreadQueue.QueueFunctionWait([=]()
	{	
		renderMaterials.Remove(this);
		Log("material destroyed render part: %p", this);
	});
	Log("material destroyed core part: %p", this);
}
