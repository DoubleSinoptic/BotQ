#include "..\MeshRenderer.h"
#include "Sge2Common.h"
#include "GameInstance.h"



MeshVariant::MeshVariant(Mesh * varic) :
	m_transofrms_nva(nullptr), m_vertex_array(nullptr), m_mesh(varic), _deletedFlag(false)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		m_vertex_array = new Gl3dLayoutInstance();

		Gl3dLayoutDesc desc;
		desc.index = m_mesh->GetMeshInfo().indecesBuffer.Get<Gl3dArray<unsigned int>*>();
		desc.layouts[0] = { m_mesh->GetMeshInfo().verticesBuffer.Get<Gl3dArray<Vector3>*>(), 3, Gl3dFundamentalType::Float, sizeof(Vector3), 0, false };
		desc.layouts[1] = { m_mesh->GetMeshInfo().normalsBuffer.Get<Gl3dArray<Vector3>*>(), 3, Gl3dFundamentalType::Float, sizeof(Vector3), 0, false };
		desc.layouts[2] = { m_mesh->GetMeshInfo().texcoordsBuffer.Get<Gl3dArray<Vector2>*>(), 2, Gl3dFundamentalType::Float, sizeof(Vector2), 0, false };
		desc.layouts[3] = { m_mesh->GetMeshInfo().tangetsBuffer.Get<Gl3dArray<Vector3>*>(), 3, Gl3dFundamentalType::Float, sizeof(Vector3), 0, false };
	
#ifdef USE_INSTANCING
		m_transofrms_nva = new Gl3dArray<Matrix4>(Gl3dArrayTarget::Array);
		desc.layouts[4] = { m_transofrms_nva, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 0, true };
		desc.layouts[5] = { m_transofrms_nva, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 1, true };
		desc.layouts[6] = { m_transofrms_nva, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 2, true };
		desc.layouts[7] = { m_transofrms_nva, 4, Gl3dFundamentalType::Float, sizeof(Vector4) * 4, sizeof(Vector4) * 3, true };
#else
		m_transofrms_nva = new DynamicArray<Matrix4>();
#endif	
		m_vertex_array->Create(&desc);
	});
	
}

void MeshVariant::QueryAdd(MeshRenderer * renderer)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		Matrix4 mt;
		renderer->GetGameObject()->FillMatrix4x4(mt.Data());
		renderer->id = m_renderers.Add(renderer);
		m_transofrms_nva->Add(mt * Matrix4::Scale(renderer->scale));
	});
}

void MeshVariant::QueryRemove(MeshRenderer * renderer)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		MeshRenderer* change = m_renderers.Back();
		change->id = m_renderers.Remove(renderer);
		m_transofrms_nva->RemoveAt(change->id);
	});
}

void MeshVariant::QueryChange(MeshRenderer * renderer)
{
	Matrix4 mt;
	renderer->GetGameObject()->FillMatrix4x4(mt.Data());
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunction([=]()
	{		
#ifdef USE_INSTANCING
		m_transofrms_nva->Set(renderer->id, mt * Matrix4::Scale(renderer->scale));
#else
		m_transofrms_nva->operator[](renderer->id) = (mt * Matrix4::Scale(renderer->scale));
#endif
	});
	/*renderer->GetGameObject()->FillMatrix4x4((float*)&renderer->updateCommand.m_transform);

	GameInstance::GetCurrent()->renderThreadQueue.Queue(&renderer->updateCommand);*/
}

MeshVariant::~MeshVariant()
{
	Assert(!_deletedFlag);
	_deletedFlag = true;
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([=]()
	{
		if (m_vertex_array)
			delete m_vertex_array;
		if (m_transofrms_nva)
			delete m_transofrms_nva;
	});
}

Mesh::Mesh() : _deletedFlag(false)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		vertices_buffer.Set(new Gl3dArray<Vector3>(Gl3dArrayTarget::Array));
		normals_buffer.Set(new Gl3dArray<Vector3>(Gl3dArrayTarget::Array));
		texcoords_buffer.Set(new Gl3dArray<Vector2>(Gl3dArrayTarget::Array));
		tangets_buffer.Set(new Gl3dArray<Vector3>(Gl3dArrayTarget::Array));
		indeces_buffer.Set(new Gl3dArray<unsigned int>(Gl3dArrayTarget::Element));
	});
}

Mesh::~Mesh()
{
	Assert(!_deletedFlag);
	_deletedFlag = true;
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		if (!vertices_buffer.IsNull())
			delete vertices_buffer.Get<Gl3dArray<Vector3>*>();
		if (!normals_buffer.IsNull())
			delete normals_buffer.Get<Gl3dArray<Vector3>*>();
		if (!tangets_buffer.IsNull())
			delete tangets_buffer.Get<Gl3dArray<Vector3>*>();
		if (!texcoords_buffer.IsNull())
			delete texcoords_buffer.Get<Gl3dArray<Vector3>*>();
		if (!indeces_buffer.IsNull())
			delete indeces_buffer.Get<Gl3dArray<Vector3>*>();
	});
}

void Mesh::applay_data()
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		vertices_buffer.Get<Gl3dArray<Vector3>*>()->Clear();
		vertices_buffer.Get<Gl3dArray<Vector3>*>()->Add(vertices.begin(), vertices.end());
		vertices_buffer.Get<Gl3dArray<Vector3>*>()->DetachRamData();

		normals_buffer.Get<Gl3dArray<Vector3>*>()->Clear();
		normals_buffer.Get<Gl3dArray<Vector3>*>()->Add(normals.begin(), normals.end());
		normals_buffer.Get<Gl3dArray<Vector3>*>()->DetachRamData();

		tangets_buffer.Get<Gl3dArray<Vector3>*>()->Clear();
		tangets_buffer.Get<Gl3dArray<Vector3>*>()->Add(tangets.begin(), tangets.end());
		tangets_buffer.Get<Gl3dArray<Vector3>*>()->DetachRamData();

		texcoords_buffer.Get<Gl3dArray<Vector2>*>()->Clear();
		texcoords_buffer.Get<Gl3dArray<Vector2>*>()->Add(texcoords.begin(), texcoords.end());
		texcoords_buffer.Get<Gl3dArray<Vector2>*>()->DetachRamData();

		indeces_buffer.Get<Gl3dArray<unsigned int>*>()->Clear();
		indeces_buffer.Get<Gl3dArray<unsigned int>*>()->Add(indeces.begin(), indeces.end());
		indeces_buffer.Get<Gl3dArray<unsigned int>*>()->DetachRamData();
	});
}

Material::Material() : _deletedFlag(false)
{
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		GameInstance::GetCurrent()->materials.Add(this);
	});
}

Material::~Material()
{
	Assert(!_deletedFlag);
	_deletedFlag = true;
	GameInstance::GetCurrent()->renderThreadQueue.QueueFunctionWait([&]()
	{
		GameInstance::GetCurrent()->materials.Remove(this);
	});
}

void Material::TryDeleteVariant(const Ref<MeshVariant>& var)
{
	Log("once variant deleted");
	if (var->m_renderers.Length() == 0)
		render_variant.Remove(var);
}

Ref<MeshVariant> Material::GetOrAddVariant(Mesh * msh)
{
	int i = -1;
	for (int j = 0; j < render_variant.Length(); j++)
		if (render_variant[j]->m_mesh == msh)
		{
			i = j;
			break;
		}
	if (i == -1)
	{
		Ref<MeshVariant> var = new MeshVariant(msh);
		i = render_variant.Add(var);
	}
	return render_variant[i];
}

Mesh * MeshRenderer::GetMesh() const
{
	return m_mesh;
}

void MeshRenderer::EnableInstancing(bool value)
{
}

void MeshRenderer::SetMesh(Mesh * msh)
{
	Material* safe = m_material;
	SetMaterial(nullptr);
	m_mesh = msh;
	SetMaterial(safe);
}
