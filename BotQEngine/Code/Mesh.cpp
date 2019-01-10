#include "Mesh.h"
#include "GameInstance.h"
#include "Gl3dCore.h"
#include "Gl3dArray.h"

void Mesh::FreeRamData()
{
	m_vntCloud[0].Realase();
	m_vntCloud[1].Realase();
	m_vntCloud[2].Realase();
	m_tCloud.Realase();
	m_iCloud.Realase();
	m_vnttCloudLength = 0;
	m_iCloudLength = 0;
}

const MeshInfo& Mesh::GetMeshInfo() const
{
	return m_info;
}

size_t Mesh::GetVertexesSize() const
{
	return m_vnttCloudLength;
}

size_t Mesh::GetIncdecesSize() const
{
	return m_iCloudLength;
}

const Vector3* Mesh::GetVertexes() const
{
	return m_vntCloud[0].GetPtr();
}

const Vector3* Mesh::GetNormals() const
{
	return m_vntCloud[1].GetPtr();
}

const Vector3* Mesh::GetTangets() const
{
	return m_vntCloud[2].GetPtr();
}

const Vector2* Mesh::GetTexcoords() const
{
	return m_tCloud.GetPtr();
}
const unsigned int* Mesh::GetIncdeces() const
{
	return m_iCloud.GetPtr();
}

void Mesh::SetVertexes(const Vector3* data, size_t count)
{
	Ref<Vector3[]> copy = new Vector3[count];
	memcpy(copy.GetPtr(), data, count * sizeof(Vector3));
	renderThreadQueue.QueueFunction([=]()
	{
		m_info.verticesBuffer.Get<Gl3dArray<Vector3>*>()->Clear();
		m_info.verticesBuffer.Get<Gl3dArray<Vector3>*>()->Add(copy.GetPtr(), count);
		m_info.verticesBuffer.Get<Gl3dArray<Vector3>*>()->DetachRamData();
	});
	m_vnttCloudLength = count;
	m_vntCloud[0] = copy;
}

void Mesh::SetNormals(const Vector3* data, size_t count)
{
	Ref<Vector3[]> copy = new Vector3[count];
	memcpy(copy.GetPtr(), data, count * sizeof(Vector3));
	renderThreadQueue.QueueFunction([=]()
	{
		m_info.normalsBuffer.Get<Gl3dArray<Vector3>*>()->Clear();
		m_info.normalsBuffer.Get<Gl3dArray<Vector3>*>()->Add(copy.GetPtr(), count);
		m_info.normalsBuffer.Get<Gl3dArray<Vector3>*>()->DetachRamData();
	});
	m_vnttCloudLength = count;
	m_vntCloud[1] = copy;
}

void Mesh::SetTangents(const Vector3* data, size_t count)
{
	Ref<Vector3[]> copy = new Vector3[count];
	memcpy(copy.GetPtr(), data, count * sizeof(Vector3));
	renderThreadQueue.QueueFunction([=]()
	{
		m_info.tangetsBuffer.Get<Gl3dArray<Vector3>*>()->Clear();
		m_info.tangetsBuffer.Get<Gl3dArray<Vector3>*>()->Add(copy.GetPtr(), count);
		m_info.tangetsBuffer.Get<Gl3dArray<Vector3>*>()->DetachRamData();
	});
	m_vnttCloudLength = count;
	m_vntCloud[2] = copy;
}


void Mesh::SetTexcoords(const Vector2* data, size_t count)
{
	Ref<Vector2[]> copy = new Vector2[count];
	memcpy(copy.GetPtr(), data, count * sizeof(Vector2));
	renderThreadQueue.QueueFunction([=]()
	{
		m_info.texcoordsBuffer.Get<Gl3dArray<Vector2>*>()->Clear();
		m_info.texcoordsBuffer.Get<Gl3dArray<Vector2>*>()->Add(copy.GetPtr(), count);
		m_info.texcoordsBuffer.Get<Gl3dArray<Vector2>*>()->DetachRamData();
	});
	m_tCloud = copy;
	m_vnttCloudLength = count;
}

void Mesh::SetIndeces(const unsigned int* data, size_t count)
{
	Ref<unsigned int[]> copy = new unsigned int[count];
	memcpy(copy.GetPtr(), data, count * sizeof(unsigned int));
	renderThreadQueue.QueueFunction([=]()
	{
		m_info.indecesBuffer.Get<Gl3dArray<unsigned int>*>()->Clear();
		m_info.indecesBuffer.Get<Gl3dArray<unsigned int>*>()->Add(copy.GetPtr(), count);
		m_info.indecesBuffer.Get<Gl3dArray<unsigned int>*>()->DetachRamData();
	});
	m_iCloud = copy;
	m_iCloudLength = count;
}

Mesh::Mesh()
{
	renderThreadQueue.QueueFunction([=]()
	{
		m_info.verticesBuffer.Set(new Gl3dArray<Vector3>(Gl3dArrayTarget::Array));
		m_info.normalsBuffer.Set(new Gl3dArray<Vector3>(Gl3dArrayTarget::Array));
		m_info.texcoordsBuffer.Set(new Gl3dArray<Vector2>(Gl3dArrayTarget::Array));
		m_info.tangetsBuffer.Set(new Gl3dArray<Vector3>(Gl3dArrayTarget::Array));
		m_info.indecesBuffer.Set(new Gl3dArray<unsigned int>(Gl3dArrayTarget::Element));
	});
}

Mesh::~Mesh()
{
	MeshInfo duplicate = m_info;
	renderThreadQueue.QueueFunctionWait([=]()
	{
		if (!duplicate.verticesBuffer.IsNull())
			delete duplicate.verticesBuffer.Get<Gl3dArray<Vector3>*>();
		if (!duplicate.normalsBuffer.IsNull())
			delete duplicate.normalsBuffer.Get<Gl3dArray<Vector3>*>();
		if (!duplicate.tangetsBuffer.IsNull())
			delete duplicate.tangetsBuffer.Get<Gl3dArray<Vector3>*>();
		if (!duplicate.texcoordsBuffer.IsNull())
			delete duplicate.texcoordsBuffer.Get<Gl3dArray<Vector3>*>();
		if (!duplicate.indecesBuffer.IsNull())
			delete duplicate.indecesBuffer.Get<Gl3dArray<Vector3>*>();
	});
}