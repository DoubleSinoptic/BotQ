#pragma once

#include "Component.h"
#include "RenderSource.h"
#include "Mesh.h"
#include "Math/Matrix4.h"

class Material;
class Mesh;

class SGE_EXPORT MeshRenderer : public Component
{
	EventHandler<>  m_transformChanged;
	unsigned int	m_id;
	Material*		m_material;
	Mesh*			m_mesh;
	bool			m_instancing;
	RenderSource*   m_source;
	Matrix4			m_transform;
	Matrix4			m_scale;
	void UpdateStates(Material*	 material, Mesh* mesh, bool instancing);
public:
	MeshRenderer();
	~MeshRenderer();

	void Awake() override;
	void Destroy() override;
	void FrameUpdate() override;

	const Matrix4& GetScale() const;
	void SetScale(const Vector3& scale);

	const Matrix4& GetTransform() const;
	void SetDataId(unsigned int id);
	unsigned int GetDataId() const;

	void SetMaterial(Material* material);
	Material* GetMaterial() const;

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

	void EnableInstancing(bool value = true);

	void CloneTo(TypedObject* obj) const override;
};

