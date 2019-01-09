#pragma once

#include "Component.h"

class Material;
class Mesh;

class MeshRenderer : public Component
{
	unsigned int	m_id;
	Material*		m_material;
	Mesh*			m_mesh;
	bool			m_instancing;
	RenderSource*   m_source;
	Matrix4			m_transform;

	void UpdateStates(Material*	 material, Mesh* mesh, bool instancing);
public:
	MeshRenderer();
	~MeshRenderer();

	const Matrix4& GetTransform() const;
	void SetDataId(unsigned int id);
	unsigned int GetDataId() const;

	void SetMaterial(Material* material);
	Material* GetMaterial() const;

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

	void EnableInstancing(bool value = true);

};

