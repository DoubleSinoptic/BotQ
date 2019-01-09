#pragma once

#include "Component.h"
#include "GameObject.h"
#include "DigitalContainer.h"
#include "Common/DynamicArray.h"
#include "Common/Assert.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "CommandQueue.h"
#include "Common/DataVector.h"
#include "GameInstance.h"

struct MeshInfo
{
	DigitalContainer verticesBuffer;
	DigitalContainer normalsBuffer;
	DigitalContainer texcoordsBuffer;
	DigitalContainer indecesBuffer;
	DigitalContainer tangetsBuffer;
};

class SGE_EXPORT Mesh
{
	MeshInfo			m_info;
	size_t              m_vnttCloudLength;
	Ref<Vector3[]>      m_vntCloud[3];
	Ref<Vector2[]>      m_tCloud;

	Ref<unsigned int[]> m_iCloud;
	size_t              m_iCloudLength;
public:
	void FreeRamData();
	const MeshInfo& GetMeshInfo() const;
	size_t GetVertexesSize() const;
	size_t GetIncdecesSize() const;
	const Vector3* GetVertexes() const;
	const Vector3* GetNormals() const;

	const Vector3* GetTangets() const;
	const Vector2* GetTexcoords() const;
	const unsigned int* GetIncdeces() const;

	void SetVertexes(const Vector3* data, size_t count);
	void SetNormals(const Vector3* data, size_t count);
	void SetTangents(const Vector3* data, size_t count);
	void SetTexcoords(const Vector2* data, size_t count);
	void SetIndeces(const unsigned int* data, size_t count);

	Mesh();

	~Mesh();
};
