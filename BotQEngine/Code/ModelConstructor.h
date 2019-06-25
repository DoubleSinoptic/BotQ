#pragma once

#include "Core.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"

struct SGE_EXPORT IndexedMesh
{
	int m_mat_id;
	Ref<Mesh> m_mesh;
};
																																																																													
struct SGE_EXPORT ModelConstructor
{
	Vector3								m_local_position;
	Quaternion							m_local_rotation;
	DynamicArray<Ref<ModelConstructor>> m_childs;
	DynamicArray<IndexedMesh>			m_meshs;
	String								m_name;
	Ref<DynamicArray<Ref<Material>>>	m_materials;


	GameObject* Construct(GameObject* parent = nullptr);
};