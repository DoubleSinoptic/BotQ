#include "RenderSource.h"

void RenderSource::Draw(MeshStateUpdater* updater, RenderThreadTag)
{
	for (size_t i = 0; i < m_dynamicRenderes.LengthReference; i++)
	{
		MeshRenderer* renderer = m_dynamicRenderes[i];
		if (updater->Update(renderer->GetTransform()))
			m_layout->DrawIndexed(
				Gl3dPrimitive::Triangles,
				Gl3dFundamentalType::UInteger,
				m_mesh->GetIncdecesSize());
	}
}

void RenderSource::DrawInstanced(RenderThreadTag)
{
	m_instancedLayout->DrawIndexedInstanced(
		Gl3dPrimitive::Triangles, 
		Gl3dFundamentalType::UInteger, 
		m_mesh->GetIncdecesSize(), 
		m_instancedData->Length());
}

void RenderSource::Add(MeshRenderer* r)
{
	renderThreadQueue.QueueFunction([=]()
	{
		m_dynamicRenderes.Add(r);
	});
}

void RenderSource::Remove(MeshRenderer* r)
{
	renderThreadQueue.QueueFunction([=]()
	{
		m_dynamicRenderes.Remove(r);
	});
}

void RenderSource::ChangeInstanced(unsigned int id, const Matrix4& transform)
{
	renderThreadQueue.QueueFunction([=]()
	{
		m_instancedData->Set(id, transform);
	});
}

void RenderSource::AddInstanced(MeshRenderer* r, const Matrix4& transform)
{
	renderThreadQueue.QueueFunction([=]()
	{
		r->SetDataId(m_instancedRenderes.Add(r));
		m_instancedData->Add(transform);
	});
}

void RenderSource::RemoveInstanced(MeshRenderer* r)
{
	renderThreadQueue.QueueFunction([&]()
	{
		unsigned int id = r->GetDataId();
		m_instancedRenderes.RemoveAt(id);
		m_instancedData->RemoveAt(id);
		m_instancedRenderes[id]->SetDataId(id);
	});
}

RenderSource::RenderSource(Mesh* m) :
	m_mesh(m)
{
	renderThreadQueue.QueueFunction([&]()
	{
		m_instancedData = new Gl3dArray<Matrix4>(Gl3dArrayTarget::Array);
		m_instancedLayout = new Gl3dLayoutInstance();
		m_layout = new Gl3dLayoutInstance();

		Gl3dLayoutDesc desc;
		desc.index = m_mesh->GetMeshInfo().indecesBuffer.Get<Gl3dArray<unsigned int>*>();
		desc.layouts[0] = { m_mesh->GetMeshInfo().verticesBuffer.Get<Gl3dArray<Vector3>*>(), 3, Gl3dFundamentalType::Float, sizeof(Vector3), 0, false };
		desc.layouts[1] = { m_mesh->GetMeshInfo().normalsBuffer.Get<Gl3dArray<Vector3>*>(), 3, Gl3dFundamentalType::Float, sizeof(Vector3), 0, false };
		desc.layouts[2] = { m_mesh->GetMeshInfo().texcoordsBuffer.Get<Gl3dArray<Vector2>*>(), 2, Gl3dFundamentalType::Float, sizeof(Vector2), 0, false };
		desc.layouts[3] = { m_mesh->GetMeshInfo().tangetsBuffer.Get<Gl3dArray<Vector3>*>(), 3, Gl3dFundamentalType::Float, sizeof(Vector3), 0, false };

		m_layout->Create(&desc);

		desc.layouts[4] = { m_instancedData, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 0, true };
		desc.layouts[5] = { m_instancedData, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 1, true };
		desc.layouts[6] = { m_instancedData, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 2, true };
		desc.layouts[7] = { m_instancedData, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 3, true };

		m_instancedLayout->Create(&desc);
	});

	Log("RenderSource created: %p", m_mesh);
}

RenderSource::~RenderSource()
{
	renderThreadQueue.QueueFunctionWait([&]()
	{
		delete m_layout;
		delete m_instancedLayout;
		delete m_instancedData;
	});

	Log("RenderSource destroyed: %p", m_mesh);
}