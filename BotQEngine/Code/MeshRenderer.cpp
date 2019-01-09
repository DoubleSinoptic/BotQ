#include "MeshRenderer.h"
#include "RenderSource.h"
#include "GameInstance.h"

void MeshRenderer::UpdateStates(Material * material, Mesh * mesh, bool instancing)
{
	if (m_source)
	{
		m_material->NotifyLeaveSource(mesh);
		m_source = nullptr;
		m_material = nullptr;
		m_mesh = nullptr;
	}

	if (material != nullptr && mesh != nullptr)
	{
		m_source = m_material->QuerySource(mesh);
		if (m_source != nullptr)
		{
			m_material = material;
			m_mesh = mesh;
		}
	}
}

const Matrix4 & MeshRenderer::GetTransform() const
{
	return m_transform;
}

void MeshRenderer::SetDataId(unsigned int id)
{
	m_id = id;
}

unsigned int MeshRenderer::GetDataId() const 
{
	return m_id;
}

MeshRenderer::MeshRenderer() :
		m_id(0),
		m_material(nullptr),
		m_mesh(nullptr),
		m_instancing(false),
		m_source(nullptr)
{}

void MeshRenderer::SetMaterial(Material* material)
{
	UpdateStates(material, m_mesh, m_instancing);
}

Material* MeshRenderer::GetMaterial() const
{
	return m_material;
}

void MeshRenderer::SetMesh(Mesh* mesh)
{
	UpdateStates(m_material, mesh, m_instancing);
}

Mesh* MeshRenderer::GetMesh() const
{
	return m_mesh;
}

void MeshRenderer::EnableInstancing(bool value = true)
{
	UpdateStates(m_material, m_mesh, value);
}

MeshRenderer::~MeshRenderer()
{
	UpdateStates(nullptr, nullptr, m_instancing);
}

