#include "ModelConstructor.h"

//const String& ModelConstructor::GetName() const
//{
//	return m_name;
//}

GameObject* ModelConstructor::Construct(GameObject* parent)
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
		i->Construct(o);
	return o;
}