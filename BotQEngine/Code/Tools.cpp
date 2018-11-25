#include "Tools.h"
#include "GameObject.h"
#include "Graphics/Texture.h"
#include "Sge2Common.h"
#include "Time.hpp"


void Tools::SetMaterial(GameObject *cube, Material* m)
{
	for (GameObject* c : *cube)
		SetMaterial(c, m);

	DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();
	for (auto& i : r)
	{
		i->SetMaterial(m);	
	}

}
void Tools::SetMRAO(GameObject* cube, float m, float r, float ao)
{
	for (GameObject* c : *cube)
		SetMRAO(c,  m,  r,  ao);

	DynamicArray<MeshRenderer*> re = cube->GetComponents<MeshRenderer>();
	for (auto& i : re)
	{

		static_cast<ObjectMaterial*>(i->GetMaterial())->metalic_roughness_ao_static = Vector3(m, r, ao);

	}

}
void Tools::SetMaps(GameObject *cube, Texture *k, Texture *n, Texture *mt, Texture *rt, Texture *ao)
{
    for (GameObject* c : *cube)
        SetMaps(c, k, n, mt, rt, ao);

    DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();
    for (auto& i : r)
    {     
            static_cast<ObjectMaterial*>(i->GetMaterial())->albedo =(k);
			static_cast<ObjectMaterial*>(i->GetMaterial())->normals =(n);
			static_cast<ObjectMaterial*>(i->GetMaterial())->metalic = mt;
			static_cast<ObjectMaterial*>(i->GetMaterial())->roughness = rt;
			static_cast<ObjectMaterial*>(i->GetMaterial())->ao = ao;
        
    }

}

void Tools::ScaleAll(GameObject *cube, float scale)
{
    for (GameObject* c : *cube)
        ScaleAll(c, scale);

	DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();
	for (auto& i : r)
    {
        i->SetScale(Vector3(scale, scale, scale));
    }
	cube->OnTransformChanged();
}

void Tools::ScaleAll(GameObject * cube, const Vector3 & vec)
{
	for (GameObject* c : *cube)
		ScaleAll(c, vec);

	DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();

	for (auto& i : r)
	{
		i->SetScale(vec);
	}
	cube->OnTransformChanged();
}

void FixedRotation::Awake() 
{
	SetEnabled(true);
}

void FixedRotation::PhysicUpdate()
{
    GetGameObject()->SetLocalRotation(Quaternion(0.0, Time::GetTotalTime() * (Mathf::Pi() / 6 ) * RAD_TO_DEG , 0.0));
}

void Tools::RotateLocalAll(class GameObject* cube, const Quaternion& rot)
{
	for (GameObject* c : *cube)
		RotateLocalAll(c, rot);
	cube->SetLocalRotation(rot);
}