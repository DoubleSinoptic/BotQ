#pragma once

#include "Common/DynamicArray.h"
#include "Mesh.h"

class Texture;
class RenderSource;

class SGE_EXPORT Material
{
	DynamicArray<RenderSource*> m_renderSources;
public:
	Material();
	virtual ~Material();

	RenderSource* QuerySource(Mesh* r);
	void NotifyLeaveSource(RenderSource* r);

	const DynamicArray<RenderSource*>& GetRenderSources() const;
};

class SGE_EXPORT ObjectMaterial : public Material
{
public:
	Texture * metalic = nullptr;
	Texture*  ao = nullptr;
	Texture*  roughness = nullptr;
	Texture*  normals = nullptr;
	Texture*  albedo = nullptr;

	Vector4	  albedo_static = { 1.0, 1.0, 1.0, 1.0 };
	Vector3   metalic_roughness_ao_static = { 0.0, 1.0, 1.0 };
};