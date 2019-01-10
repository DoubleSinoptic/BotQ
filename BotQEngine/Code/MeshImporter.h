#pragma once

#include "Core.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"

struct SGE_EXPORT mesh_pair 
{
	int m_mat_id;
	Ref<Mesh> m_mesh;
};

struct SGE_EXPORT construct_info 
{
	Vector3 m_local_position;
	Quaternion m_local_rotation;
	DynamicArray<Ref<construct_info>> m_childs;
	DynamicArray<mesh_pair>	m_meshs;
	String m_name;
	Ref<DynamicArray<Ref<Material>>> m_materials;
	GameObject* construct(GameObject* parent = nullptr) 
	{
		GameObject* o = new GameObject();
		o->SetParent(parent);
		o->SetName(m_name);
		o->SetLocalPosition(m_local_position);
		o->SetLocalRotation(m_local_rotation);
		for (auto i : m_meshs) 
		{
			MeshRenderer* r = o->AddComponent<MeshRenderer>();
			r->SetMesh(i.m_mesh.GetPtr());
			r->SetMaterial(m_materials->operator[](i.m_mat_id).GetPtr());
		}
		for (auto i : m_childs)
			i->construct(o);
		return o;
	}
};


class SGE_EXPORT MeshImporter
{
public:
	static Ref<construct_info> Import(String file/*, const String& txtureAdditionalPath = ""*/);
	//{
	//	return Ref<construct_info>
	//		();
	//}
	static Ref<construct_info> ImportRW(String path, const char* data, size_t len/*, const String& txtureAdditionalPath = ""*/);
	//{
	//	return Ref<construct_info>
	//		();
	//}
};

