#pragma once

class Material
{
	DynamicArray<RenderSource*> m_renderSources;
public:
	Material();
	virtual ~Material();

	RenderSource* QuerySource(Mesh* r);
	void NotifyLeaveSource(Mesh* r);

	const DynamicArray<RenderSource*>& GetRenderSources() const;
};

class PbrMaterial : public Material
{
public:
};