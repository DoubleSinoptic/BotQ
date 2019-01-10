#include "MeshRenderer.h"
#include "RenderSource.h"
#include "GameInstance.h"

void MeshRenderer::Awake()  
{
	m_scale = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f));
	SetEnabled(true);
	m_transformChanged = EventHandler<>([&]()
	{
		if (m_instancing && m_source)
		{
			Matrix4 fl;
			GetGameObject()->FillMatrix4x4((float*)&fl);
			m_source->ChangeInstanced(m_id, fl * m_scale);
		}
	}, false);
	GetGameObject()->GetHandlers()->OnTransformChanged += &m_transformChanged;
}

void MeshRenderer::Destroy() 
{
	GetGameObject()->GetHandlers()->OnTransformChanged -= &m_transformChanged;
}

void MeshRenderer::FrameUpdate()
{
	Matrix4 fl;
	GetGameObject()->FillMatrix4x4((float*)&fl);
	m_transform = fl * m_scale;
}

const Matrix4& MeshRenderer::GetScale() const
{
	return m_scale;
}

void MeshRenderer::SetScale(const Vector3& scale)
{
	m_scale = Matrix4::Scale(scale);
}

void MeshRenderer::UpdateStates(Material * material, Mesh * mesh, bool instancing)
{
	if (m_source)
	{
		if (m_instancing)
			m_source->RemoveInstanced(this);
		else
			m_source->Remove(this);
		m_material->NotifyLeaveSource(m_source);
		m_source = nullptr;
		m_material = nullptr;
		m_mesh = nullptr;
	}

	m_material = material;
	m_mesh = mesh;
	m_instancing = instancing;

	if (m_material != nullptr && m_mesh != nullptr)
		m_source = m_material->QuerySource(mesh);	

	if (m_source)
	{
		if (m_instancing)
		{
			Matrix4 fl;
			GetGameObject()->FillMatrix4x4((float*)&fl);
			m_transform = fl * m_scale;
			m_source->AddInstanced(this, m_transform);
		}
		else
		{
			m_source->Add(this);
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

void MeshRenderer::EnableInstancing(bool value)
{
	UpdateStates(m_material, m_mesh, value);
}

MeshRenderer::~MeshRenderer()
{
	UpdateStates(nullptr, nullptr, m_instancing);
}

